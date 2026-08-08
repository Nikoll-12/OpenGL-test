cmake_minimum_required(VERSION 3.20)
project(prueba_opengl LANGUAGES CXX)

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

add_executable(prueba_opengl triangulo.cpp)

target_link_libraries(prueba_opengl
    PRIVATE
        glfw
        OpenGL::GL
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)

# Avisos utiles, no obligatorios.
target_compile_options(prueba_opengl PRIVATE -Wall -Wextra)

message(STATUS "OpenGL encontrado: ${OPENGL_gl_LIBRARY}")
