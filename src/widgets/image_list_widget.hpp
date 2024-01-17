#pragma once

#include "../widget.hpp"

#include <limits>
#include <vector>

#include <cstddef>

class image_list_widget final : public widget
{
public:
  using select_callback = void (*)(void* data, std::size_t index);

  image_list_widget(void* cb_data, select_callback cb_func);

  void render(edit_moderator<project>& proj) override;

  void on_project_modification(const project& proj) override;

  void on_image_select(const project& proj, std::size_t index) override;

protected:
  void on_internal_image_select(std::size_t index);

  void render_image_list_in_modify_state(edit_moderator<project>& proj);

  void render_image_list_without_modify_state(edit_moderator<project>& proj);

  void begin_modify_state(const project& proj);

  void end_modify_state();

private:
  std::size_t m_image_index{ std::numeric_limits<std::size_t>::max() };

  void* m_cb_data{ nullptr };

  select_callback m_cb_func{ nullptr };

  bool m_modify_state{ false };

  std::vector<std::size_t> m_modify_selection;

  std::vector<unsigned char> m_modify_selection_states;
};
