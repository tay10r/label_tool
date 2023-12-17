#include "image_viewer.hpp"

#include <implot.h>

#include <array>
#include <string>
#include <vector>

#include <GLES2/gl2.h>

#include "project.hpp"

#include "deps/stb_image.h"

namespace {

class image_viewer_impl;

template<typename Value>
struct callback final
{
  image_viewer_impl* self{ nullptr };

  void (*func)(image_viewer_impl&, const Value& value){ nullptr };
};

class tool
{
public:
  virtual ~tool() = default;

  virtual void on_start() = 0;

  virtual void on_click(float x, float y) = 0;

  virtual void render_to_plot(float x, float y) = 0;
};

template<typename Value>
class tool_base : public tool
{
public:
  using callback_type = callback<Value>;

  tool_base(image_viewer_impl* self, void (*func)(image_viewer_impl&, const Value& value))
    : m_callback{ self, func }
  {
  }

protected:
  void invoke_callback(const Value& value) { m_callback.func(*m_callback.self, value); }

private:
  callback_type m_callback{};
};

class crop_tool final : public tool_base<bbox<float, 2>>
{
public:
  using tool_base<bbox<float, 2>>::tool_base;

  void on_start() override {}

  void on_click(float x, float y) override
  {
    m_state.coords.emplace_back(vec2{ x, y });
    if (m_state.coords.size() == 2) {
      bbox<float, 2> crop_bbox{ m_state.coords.at(0), m_state.coords.at(0) };
      crop_bbox.extend(m_state.coords.at(1));
      invoke_callback(crop_bbox);
      reset_state();
    }
  }

  void render_to_plot(float x, float y) override
  {
    if (!m_state.coords.empty()) {
      ImPlot::PlotScatter("##CropCornerTmp", &x, &y, 1);
    }

    if (m_state.coords.size() == 1) {
      const auto v0 = m_state.coords.at(0);
      const auto v1 = vec2{ x, y };
      std::array<float, 5> lines_x{ v0[0], v0[0], v1[0], v1[0], v0[0] };
      std::array<float, 5> lines_y{ v0[1], v1[1], v1[1], v0[1], v0[1] };
      ImPlot::PlotLine("##CropBBoxTmp", lines_x.data(), lines_y.data(), 5);
    }
  }

protected:
  using vec2 = std::array<float, 2>;

  struct state final
  {
    std::vector<vec2> coords;
  };

  void reset_state() { m_state = state(); }

private:
  callback_type m_callback{};

  state m_state;
};

class label_tool final : public tool_base<int>
{
public:
  using tool_base<int>::tool_base;

  void on_start() override {}

  void on_click(float x, float y) override {}

  void render_to_plot(float x, float y) override {}
};

class image_viewer_impl final : public image_viewer
{
public:
  image_viewer_impl(image_viewer_observer* observer)
    : m_observer(observer)
  {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_tools.emplace_back("Crop", new crop_tool(this, on_crop));
    m_tools.emplace_back("Label", new label_tool(this, on_label));
  }

  ~image_viewer_impl() { glDeleteTextures(1, &m_texture); }

  void render(project& prj) override
  {
    render_tools();

    render_frame_label_selection(prj);

    if (!ImPlot::BeginPlot("Image", ImVec2(-1, -1), ImPlotFlags_Crosshairs)) {
      return;
    }

    auto texture_id = reinterpret_cast<ImTextureID>(m_texture);

    ImPlot::PlotImage("##Selected Image", texture_id, ImPlotPoint(0, 0), ImPlotPoint(1, 1));

    if (!prj.crop_bbox.empty()) {
      const auto v0 = prj.crop_bbox.min;
      const auto v1 = prj.crop_bbox.max;
      std::array<float, 5> x{ v0[0], v0[0], v1[0], v1[0], v0[0] };
      std::array<float, 5> y{ v0[1], v1[1], v1[1], v0[1], v0[1] };
      ImPlot::PlotLine("Crop BBox", x.data(), y.data(), 5);
    }

    if (ImPlot::IsPlotHovered()) {

      const auto coords = ImPlot::GetPlotMousePos();

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        m_tools.at(m_selected_tool).second->on_click(coords.x, coords.y);
      } else {
        m_tools.at(m_selected_tool).second->render_to_plot(coords.x, coords.y);
      }
    }

    ImPlot::EndPlot();
  }

  void open_image(const char* path, int class_id) override
  {
    m_path = path;
    m_class_id = class_id;

    int w = 0;
    int h = 0;
    auto* data = stbi_load(path, &w, &h, nullptr, 3);

    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (data) {
      stbi_image_free(data);
    }
  }

protected:
  static void on_label(image_viewer_impl& self, const int& label)
  {
    //
  }

  static void on_crop(image_viewer_impl& self, const bbox<float, 2>& crop_bbox)
  {
    self.m_observer->on_crop_bbox_change(crop_bbox);
  }

  void render_frame_label_selection(const project& prj)
  {
    const auto* current = prj.get_frame_class_id(m_class_id);

    if (!ImGui::BeginCombo("Frame Label", current)) {
      return;
    }

    for (const auto& class_def_inst : prj.class_defs) {
      if (ImGui::Selectable(class_def_inst.name.c_str())) {
        m_class_id = class_def_inst.value;
        m_observer->on_frame_class_id_change(m_path, class_def_inst.value);
      }
    }

    ImGui::EndCombo();
  }

  void render_tools()
  {
    for (std::size_t i = 0; i < m_tools.size(); i++) {

      if (ImGui::RadioButton(m_tools[i].first.c_str(), m_selected_tool == i)) {
        m_selected_tool = i;
        m_tools[i].second->on_start();
      }

      if ((i + 1) < m_tools.size()) {
        ImGui::SameLine();
      }
    }
  }

private:
  int m_class_id{ 0 };

  std::string m_path;

  GLuint m_texture{};

  std::vector<std::pair<std::string, std::unique_ptr<tool>>> m_tools;

  std::size_t m_selected_tool{ 0 };

  image_viewer_observer* m_observer{ nullptr };
};

} // namespace

auto
image_viewer::create(image_viewer_observer* obs) -> std::unique_ptr<image_viewer>
{
  return std::make_unique<image_viewer_impl>(obs);
}
