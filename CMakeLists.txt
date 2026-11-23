cmake_minimum_required(VERSION 3.20)
project(galeria LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # para que clangd / cpptools entiendan el proyecto

if(NOT APPLE)
    message(FATAL_ERROR "Este CMakeLists esta escrito especificamente para macOS.")
endif()

# OpenGL.framework: sigue presente en el SDK de macOS aunque este deprecado.
find_package(OpenGL REQUIRED)

# GLFW instalado con: brew install glfw
# En Apple Silicon Homebrew vive en /opt/homebrew, no en /usr/local.
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

add_executable(galeria galeria.cpp)

target_link_libraries(galeria
    PRIVATE
        glfw
        glm::glm
        OpenGL::GL
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

# Avisos utiles, no obligatorios.
target_compile_options(galeria PRIVATE -Wall -Wextra)

message(STATUS "OpenGL encontrado: ${OPENGL_gl_LIBRARY}")
