#include "task.hpp"

task::task(std::size_t num_steps) noexcept
  : m_num_steps(num_steps)
{
}

auto
task::done() const -> bool
{
  return m_step_index >= m_num_steps;
}

void
task::step()
{
  if (done()) {
    return;
  }

  do_step();

  m_step_index++;
}
