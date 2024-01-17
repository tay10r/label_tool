#pragma once

#include <vector>

#include <cstddef>

template<typename T>
class edit_moderator final
{
public:
  using size_type = std::size_t;

  edit_moderator(T init = T{})
    : m_history({ std::move(init) })
  {
  }

  edit_moderator(const edit_moderator&) = delete;

  edit_moderator(edit_moderator&&) = default;

  auto operator=(const edit_moderator&) -> edit_moderator& = delete;

  auto operator=(edit_moderator&&) -> edit_moderator& = default;

  void undo()
  {
    if (m_offset > 0) {
      m_offset--;
    }
  }

  void redo()
  {
    if ((m_offset + 1) < m_history.size()) {
      m_offset++;
    }
  }

  auto edit() -> T&
  {
    m_changed = true;
    T item(m_history.at(m_offset));
    m_history.resize(m_offset + 1);
    m_history.emplace_back(std::move(item));
    m_offset++;
    return m_history.back();
  }

  auto get() const -> const T& { return m_history.at(m_offset); }

  auto has_unsaved_changes() const -> bool { return (m_offset != 0); }

  auto changed() const -> bool { return m_changed; }

  void ack_change() { m_changed = false; }

private:
  std::vector<T> m_history;

  std::size_t m_offset{ 0 };

  bool m_changed{ false };
};
