cmake_minimum_required(VERSION 3.18)
project(PDFBot LANGUAGES CXX)




set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(/std:c++latest)


# Force CMake to use your specific vcpkg installation
set(CMAKE_TOOLCHAIN_FILE
    "C:/Users/Owner/OneDrive/Desktop/SDLIntro/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file"
)


# Find packages installed with vcpkg
 find_package(podofo CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)



# Executable
add_executable(PDFBot
    src/main.cpp
 src/PDFParser.cpp)

 target_include_directories(PDFBot PRIVATE include)

# Link dependencies
target_link_libraries(PDFBot PRIVATE
    glfw
    imgui::imgui
    glad::glad
    podofo::podofo
)

# Optional: For MSVC warnings
if (MSVC)
    target_compile_options(PDFBot PRIVATE /W4 /permissive-)
else()
    target_compile_options(PDFBot PRIVATE -Wall -Wextra -Wpedantic)
endif()
