cmake_minimum_required(VERSION 3.16)
project(UniversalEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(UniversalEngine ${SOURCES} ${TEST_SOURCES})

target_include_directories(UniversalEngine PRIVATE src tests)

target_link_libraries(UniversalEngine 
    OpenGL::GL 
    glfw
    GLEW::GLEW
    glm::glm
)

if(WIN32)
    target_link_libraries(UniversalEngine opengl32)
endif()
