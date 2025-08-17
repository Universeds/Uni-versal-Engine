cmake_minimum_required(VERSION 3.16)
project(UniversalEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0110 NEW)

# Set vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

find_package(OpenGL REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(UniversalEngineLib ${SOURCES})
target_include_directories(UniversalEngineLib PUBLIC src)
target_link_libraries(UniversalEngineLib 
    OpenGL::GL 
    glfw
    GLEW::GLEW
    glm::glm
)

if(WIN32)
    target_link_libraries(UniversalEngineLib opengl32)
endif()

add_executable(UniversalEngine src/main.cpp)
target_link_libraries(UniversalEngine UniversalEngineLib)
