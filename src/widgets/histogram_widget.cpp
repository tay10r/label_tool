#include "histogram_widget.hpp"

#include "../edit_moderator.hpp"
#include "../histogram.hpp"
#include "../project.hpp"

#include <implot.h>

namespace {

class histogram_widget_impl final : public histogram_widget
{
public:
  void render(edit_moderator<project>& proj) override
  {
    if (!ImPlot::BeginPlot("Histogram", ImVec2(-1, -1))) {
      return;
    }

    ImPlot::SetupAxes("Bin", "Count");

    ImPlot::SetupAxisLimits(ImAxis_X1, 0, 256, ImPlotCond_Once);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 0.1, ImPlotCond_Once);

    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0, 0, 1));
    ImPlot::PlotLine("Red", m_histogram.r.data(), m_histogram.r.size());

    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0, 1, 0, 1));
    ImPlot::PlotLine("Green", m_histogram.g.data(), m_histogram.g.size());

    ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0, 0, 1, 1));
    ImPlot::PlotLine("Blue", m_histogram.b.data(), m_histogram.b.size());

    ImPlot::PopStyleColor(3);

    ImPlot::EndPlot();
  }

  void on_image_select(const project& proj, std::size_t image_index) override
  {
    if (image_index >= proj.frames.size()) {
      m_histogram = histogram{};
    } else {
      open_image(proj.frames.at(image_index).path.string().c_str(), proj.crop_bbox);
    }
  }

  void on_project_modification(const project& proj) override {}

protected:
  void open_image(const char* path, const bbox<float, 2>& bounds)
  {
    m_histogram = histogram::from_image_path(path, bounds);
  }

private:
  histogram m_histogram;
};

} // namespace

auto
histogram_widget::create() -> std::unique_ptr<histogram_widget>
{
  return std::make_unique<histogram_widget_impl>();
}
