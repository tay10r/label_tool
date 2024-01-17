cmake_minimum_required(VERSION 3.14.7)

project(label_tool
  DESCRIPTION "A tool for creating labeled image datasets for machine learning."
  VERSION 0.1.0)

option(LABEL_TOOL_INSTALL "Whether or not to define the install rules for the program." ON)

include(FetchContent)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(uikit URL "https://github.com/tay10r/uikit/archive/refs/tags/v0.1.3.zip")
FetchContent_MakeAvailable(uikit)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(label_tool WIN32
  src/main.cpp
  src/edit_moderator.hpp
  src/widget_manager.hpp
  src/widget_manager.cpp
  src/widget.hpp
  src/widgets/help_widget.hpp
  src/widgets/help_widget.cpp
  src/widgets/image_list_widget.hpp
  src/widgets/image_list_widget.cpp
  src/widgets/image_view_widget.hpp
  src/widgets/image_view_widget.cpp
  src/widgets/class_def_widget.hpp
  src/widgets/class_def_widget.cpp
  src/widgets/class_dist_widget.hpp
  src/widgets/class_dist_widget.cpp
  src/widgets/histogram_widget.hpp
  src/widgets/histogram_widget.cpp
  src/widgets/background_model_widget.hpp
  src/widgets/background_model_widget.cpp
  src/widgets/tasks_widget.hpp
  src/widgets/tasks_widget.cpp
  src/widgets/notes_widget.hpp
  src/widgets/notes_widget.cpp
  src/background_model.hpp
  src/background_model.cpp
  src/task.hpp
  src/task.cpp
  src/task_manager.hpp
  src/task_manager.cpp
  src/tasks/import_image_directory_task.hpp
  src/tasks/import_image_directory_task.cpp
  src/exporter_thread.hpp
  src/exporter_thread.cpp
  src/exporter.hpp
  src/exporter.cpp
  src/custom_exporter.hpp
  src/custom_exporter.cpp
  src/bbox.hpp
  src/project.hpp
  src/project.cpp
  src/histogram.hpp
  src/histogram.cpp
  src/deps/stb_image.h
  src/deps/stb_image.c
  src/deps/stb_image_write.h
  src/deps/stb_image_write.c)
target_link_libraries(label_tool PUBLIC uikit::uikit uikit::main yaml-cpp::yaml-cpp)

if(LABEL_TOOL_INSTALL)

  install(TARGETS label_tool
    DESTINATION bin)

  set (CPACK_PACKAGE_CONTACT "Taylor Holberton")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${label_tool_DESCRIPTION}")
  set (CPACK_DEBIAN_PACKAGE_DEPENDS libgl1 libc6 libx11-6)

  include(CPack)

endif()
