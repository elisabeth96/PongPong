project(PongPong)
cmake_minimum_required(VERSION 3.19)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})

# Add Corrade as a subproject
add_subdirectory(External/corrade EXCLUDE_FROM_ALL)

# Add Magnum as a subproject, enable Sdl2Application
set(WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(External/magnum EXCLUDE_FROM_ALL)

find_package(Magnum REQUIRED GL Sdl2Application)

corrade_add_resource(PP_Rcs resources.conf)

add_executable(pongpong main.cpp FullScreenTriangle.cpp FullScreenTriangle.h ${PP_Rcs})

target_link_libraries(pongpong PRIVATE
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        )