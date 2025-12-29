cmake_minimum_required(VERSION 3.16)
project(FlipperPineappleDesktopC C)
set(CMAKE_C_STANDARD 11)

# Win32 GUI app
if (WIN32)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

add_executable(FlipperPineappleDesktopC WIN32
  src/main.c
  src/ui.c
  src/serial.c
  src/flipper.c
  src/pineapple.c
  third_party/cJSON.c
)

# Link Windows libs
if (WIN32)
  target_link_libraries(FlipperPineappleDesktopC
    ws2_32
    winhttp
)
endif()

# Enable warnings
if (MSVC)
  target_compile_options(FlipperPineappleDesktopC PRIVATE /W4)
else()
  target_compile_options(FlipperPineappleDesktopC PRIVATE -Wall -Wextra)
endif()
