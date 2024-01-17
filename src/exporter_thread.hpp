#pragma once

#include "exporter.hpp"

#include <atomic>
#include <memory>
#include <thread>

class exporter_thread final
{
public:
  exporter_thread(std::unique_ptr<exporter> exp);

  auto get_remaining_frames() -> std::size_t { return m_remaining_frames.load(); }

  auto done() -> bool { return m_done.load(); }

  void cancel();

  void close();

protected:
  void run_thread();

private:
  std::unique_ptr<exporter> m_exporter;

  std::atomic<std::size_t> m_remaining_frames{ 0 };

  std::atomic<bool> m_done{ false };

  std::atomic<bool> m_cancel{ false };

  std::thread m_thread;

  bool m_closed{ false };
};
