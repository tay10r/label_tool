#pragma once

#include "exporter.hpp"

class custom_exporter final : public exporter
{
protected:
  using exporter::exporter;

  void process_frame(const project::frame& f, const std::uint8_t* rgb, std::int32_t w, std::int32_t h) override;

  void prepare() override;

  void finalize() override;

private:
  struct output_entry final
  {
    int class_id{};
  };

  std::vector<output_entry> m_output_entries;
};
