#include "exporter_thread.hpp"

exporter_thread::exporter_thread(std::unique_ptr<exporter> exp)
  : m_exporter(std::move(exp))
  , m_thread(&exporter_thread::run_thread, this)
{
}

void
exporter_thread::cancel()
{
  m_cancel.store(true);

  close();
}

void
exporter_thread::close()
{
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void
exporter_thread::run_thread()
{
  while (!m_cancel.load()) {

    if (m_exporter->get_remaining_frames() == 0) {
      break;
    }

    m_exporter->process_next_frame();

    m_remaining_frames.store(m_exporter->get_remaining_frames());
  }

  m_remaining_frames.store(0);

  m_done.store(true);
}
