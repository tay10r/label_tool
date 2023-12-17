#pragma once

#include <memory>
#include <string>

#include "bbox.hpp"
#include "project.hpp"

class image_viewer_observer
{
public:
  virtual ~image_viewer_observer() = default;

  virtual void on_crop_bbox_change(const bbox<float, 2>&) = 0;

  virtual void on_frame_class_id_change(const std::string& path, int frame_class_id) = 0;
};

class project;

class image_viewer
{
public:
  static auto create(image_viewer_observer* observer) -> std::unique_ptr<image_viewer>;

  virtual ~image_viewer() = default;

  virtual void render(project& prj) = 0;

  virtual void open_image(const char* path, int class_id) = 0;
};
