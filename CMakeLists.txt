cmake_minimum_required(VERSION 3.14.7)

project(label_tool)

include(FetchContent)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(uikit URL "https://github.com/tay10r/uikit/archive/refs/tags/v0.1.2.zip")
FetchContent_MakeAvailable(uikit)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(label_tool WIN32
  main.cpp
  exporter_thread.hpp
  exporter_thread.cpp
  exporter.hpp
  exporter.cpp
  custom_exporter.hpp
  custom_exporter.cpp
  image_viewer.hpp
  image_viewer.cpp
  bbox.hpp
  project.hpp
  project.cpp
  deps/stb_image.h
  deps/stb_image.c
  deps/stb_image_write.h
  deps/stb_image_write.c)
target_link_libraries(label_tool PUBLIC uikit::uikit uikit::main yaml-cpp::yaml-cpp)
