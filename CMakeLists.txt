project(PongPong)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/External/magnum/modules" ${CMAKE_MODULE_PATH})

# Add Corrade as a subproject
add_subdirectory(External/corrade EXCLUDE_FROM_ALL)

# Add Magnum as a subproject, enable Sdl2Application
set(WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
set(WITH_TEXT ON CACHE BOOL "" FORCE)
add_subdirectory(External/magnum EXCLUDE_FROM_ALL)

# Add MagnumPlugins as a subproject
set(WITH_FREETYPEFONT ON CACHE BOOL "" FORCE)
add_subdirectory(External/magnum-plugins EXCLUDE_FROM_ALL)

find_package(Magnum REQUIRED GL Text Shaders Sdl2Application)
find_package(MagnumPlugins REQUIRED FreeTypeFont)

corrade_add_resource(PP_Rcs resources.conf)

add_executable(pongpong main.cpp PongPongApplication.cpp PongPongApplication.h FullScreenTriangle.cpp FullScreenTriangle.h ${PP_Rcs})

target_link_libraries(pongpong PRIVATE
        Corrade::Main
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        Magnum::Text
        )

add_dependencies(pongpong MagnumPlugins::FreeTypeFont)
