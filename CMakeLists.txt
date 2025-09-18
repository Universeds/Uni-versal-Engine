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

# Add ImGui sources
file(GLOB IMGUI_SOURCES 
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(UniversalEngineLib ${SOURCES} ${IMGUI_SOURCES})
target_include_directories(UniversalEngineLib PUBLIC 
    src 
    imgui 
    imgui/backends
)
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

add_executable(UniversalEngineTests ${TEST_SOURCES})
target_include_directories(UniversalEngineTests PRIVATE src tests)
target_link_libraries(UniversalEngineTests UniversalEngineLib)

enable_testing()

add_test(NAME "Entity Creation" COMMAND UniversalEngineTests --test="Entity Creation")
add_test(NAME "Entity Validation" COMMAND UniversalEngineTests --test="Entity Validation")
add_test(NAME "Deferred Component Addition" COMMAND UniversalEngineTests --test="Deferred Component Addition")
add_test(NAME "Component Retrieval" COMMAND UniversalEngineTests --test="Component Retrieval")
add_test(NAME "Component Removal" COMMAND UniversalEngineTests --test="Component Removal")
add_test(NAME "Multiple Components" COMMAND UniversalEngineTests --test="Multiple Components")
add_test(NAME "Entity Destruction" COMMAND UniversalEngineTests --test="Entity Destruction")
add_test(NAME "Pending Operation Count" COMMAND UniversalEngineTests --test="Pending Operation Count")
