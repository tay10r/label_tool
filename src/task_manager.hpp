#pragma once

#include <memory>
#include <vector>

class task;

class task_manager
{
public:
  using completion_callback = void (*)(void*, int, task*, bool);

  static auto create() -> std::unique_ptr<task_manager>;

  virtual ~task_manager() = default;

  virtual auto queue(std::unique_ptr<task> t, void* cb_data, completion_callback cb_func) -> int = 0;

  virtual void cancel(int id) = 0;

  virtual void cancel_all() = 0;

  virtual void poll() = 0;

  virtual auto get_active() const -> std::vector<int> = 0;

  virtual auto get_queued() const -> std::vector<int> = 0;

  virtual auto get_name(int task_id) const -> const char* = 0;

  virtual auto get_completion_ratio(int task_id) const -> float = 0;
};
