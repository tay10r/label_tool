#pragma once

#include <memory>
#include <string>

#include "../bbox.hpp"
#include "../widget.hpp"

class image_view_observer
{
public:
  virtual ~image_view_observer() = default;

  virtual void on_crop_bbox_change(const bbox<float, 2>&) = 0;

  virtual void on_frame_class_id_change(const std::string& path, int frame_class_id) = 0;
};

class project;

class image_view_widget : public widget
{
public:
  static auto create(image_view_observer* observer) -> std::unique_ptr<image_view_widget>;

  image_view_widget() = default;

  image_view_widget(const image_view_widget&) = delete;

  image_view_widget(image_view_widget&&) = delete;

  auto operator=(const image_view_widget&) -> image_view_widget& = delete;

  auto operator=(image_view_widget&&) -> image_view_widget& = delete;

  virtual ~image_view_widget() = default;

  virtual void open_image(const char* path, int class_id) = 0;
};
