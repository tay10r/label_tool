#include "task_manager.hpp"

#include "task.hpp"

#include <vector>

#include <limits>

namespace {

class simple_task_manager_impl final : public task_manager
{
public:
  struct entry final
  {
    std::unique_ptr<task> task_instance;

    void* cb_data{ nullptr };

    completion_callback cb_func{ nullptr };

    int id{ 0 };
  };

  auto queue(std::unique_ptr<task> t, void* cb_data, completion_callback cb_func) -> int override
  {
    const auto id = m_next_id;

    m_entries.emplace_back(entry{ std::move(t), cb_data, cb_func, id });

    m_next_id++;

    return id;
  }

  void cancel(int id) override
  {
    for (auto it = m_entries.begin(); it != m_entries.end(); it++) {
      if (it->id == id) {
        if (it->cb_func) {
          const auto completed = false;
          it->cb_func(it->cb_data, it->id, it->task_instance.get(), completed);
        }
        m_entries.erase(it);
        break;
      }
    }
  }

  void cancel_all() override
  {
    for (auto& ent : m_entries) {
      if (ent.cb_func) {
        const auto completed = false;
        ent.cb_func(ent.cb_data, ent.id, ent.task_instance.get(), completed);
      }
    }

    m_entries.clear();
  }

  void poll() override
  {
    if (m_entries.empty()) {
      return;
    }

    auto& ent = m_entries.at(0);

    ent.task_instance->step();

    if (ent.task_instance->done()) {
      if (ent.cb_func) {
        auto completed = true;
        ent.cb_func(ent.cb_data, ent.id, ent.task_instance.get(), completed);
      }

      m_entries.erase(m_entries.begin());
    }
  }

  auto get_active() const -> std::vector<int> override
  {
    std::vector<int> active;
    if (!m_entries.empty()) {
      active.emplace_back(m_entries.at(0).id);
    }
    return active;
  }

  auto get_queued() const -> std::vector<int> override
  {
    std::vector<int> queued;

    for (std::size_t i = 1; i < m_entries.size(); i++) {
      queued.emplace_back(m_entries.at(i).id);
    }

    return queued;
  }

  auto get_name(int task_id) const -> const char* override
  {
    for (const auto& ent : m_entries) {
      if (ent.id == task_id) {
        return ent.task_instance->get_name();
      }
    }

    return "";
  }

  auto get_completion_ratio(int task_id) const -> float override
  {
    for (const auto& ent : m_entries) {
      if (ent.id == task_id) {
        const auto idx = ent.task_instance->get_step_index();
        const auto max = ent.task_instance->get_num_steps();
        if (max == std::numeric_limits<std::size_t>::max()) {
          const auto n = static_cast<float>(idx);
          return 1 - (1 / (1 + n / 100.0f));
        } else {
          return static_cast<float>(idx) / static_cast<float>(max);
        }
      }
    }
    return 0.0;
  }

private:
  std::vector<entry> m_entries;

  int m_next_id{ 0 };
};

} // namespace

auto
task_manager::create() -> std::unique_ptr<task_manager>
{
  return std::unique_ptr<task_manager>(new simple_task_manager_impl());
}
