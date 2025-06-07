cmake_minimum_required(VERSION 3.20)
project(MyRaylibProject VERSION 1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(raylib CONFIG REQUIRED)
find_path(RAYGUI_INCLUDE_DIRS "raygui.h")
find_path(PHYSAC_INCLUDE_DIRS "physac.h")
add_executable(platformer
    source/main.cpp
    source/camera.cpp
    source/collision.cpp
    source/entity.cpp
    source/quadtree.cpp
    source/random.cpp
    source/userInput.cpp
)
set_target_properties(platformer PROPERTIES
    WIN32_EXECUTABLE $<CONFIG:Release>
)
target_link_libraries(platformer PRIVATE raylib)
target_include_directories(platformer PRIVATE 
    ${RAYGUI_INCLUDE_DIRS}
    ${PHYSAC_INCLUDE_DIRS}
)