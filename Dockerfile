FROM ubuntu:22.04
RUN apt update
RUN apt install -y cmake g++ ninja-build curl unzip zip git pkg-config libpython3-dev python3 python-is-python3 python3-pip xorg-dev libxinerama-dev libxcursor-dev
RUN useradd --create-home --shell /bin/bash builder
USER builder
WORKDIR /home/builder

# Setup vcpkg for dependencies.
RUN git clone https://github.com/microsoft/vcpkg
WORKDIR vcpkg
RUN git checkout "2023.12.12"
RUN ./bootstrap-vcpkg.sh -disableMetrics
RUN ./vcpkg install yaml-cpp nlohmann-json
ENV VCPKG_ROOT=/home/builder/vcpkg
WORKDIR ..

# Setup emscripten for the web demo.
RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR emsdk
RUN git checkout "3.1.51"
RUN ./emsdk install "3.1.51"
RUN ./emsdk activate "3.1.51"
WORKDIR ..

RUN mkdir build
RUN mkdir deploy

EXPOSE 9150

CMD cmake --preset vcpkg -B build -S project -G Ninja && \
  cd build &&                                            \
  ninja &&                                               \
  cpack -G DEB &&                                        \
  cpack -G ZIP &&                                        \
  cp label_tool-* /home/builder/deploy &&                \
  cd /home/builder/deploy &&                             \
  python3 -m http.server --bind 0.0.0.0 9150
