#pragma once

#include <cstddef>

class task
{
public:
  task(std::size_t num_steps) noexcept;

  virtual ~task() = default;

  virtual auto done() const -> bool;

  void step();

  virtual auto get_name() const -> const char* = 0;

  auto get_num_steps() const -> std::size_t { return m_num_steps; }

  auto get_step_index() const -> std::size_t { return m_step_index; }

protected:
  virtual void do_step() = 0;

private:
  std::size_t m_step_index{ 0 };

  std::size_t m_num_steps{ 0 };
};
