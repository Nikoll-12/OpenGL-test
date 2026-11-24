cmake_minimum_required(VERSION 3.20)
project(galeria LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # para que clangd / cpptools entiendan el proyecto

# GLFW instalado con:
#   macOS   -> brew install glfw   (en Apple Silicon vive en /opt/homebrew)
#   Linux   -> apt install libglfw3-dev (u otro gestor de paquetes)
#   Windows -> vcpkg install glfw3 (o el instalador que uses con CMake)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

# libs/gl_loader.cpp/.h es un reemplazo chiquito de GLAD, escrito a mano: pide
# los punteros a las funciones de OpenGL moderno con glfwGetProcAddress(), que
# ya viene con GLFW y funciona igual en macOS, Windows y Linux. Por eso no
# hace falta instalar GLAD ni GLEW aparte.
add_executable(galeria galeria.cpp libs/gl_loader.cpp)

target_link_libraries(galeria
    PRIVATE
        glfw
        glm::glm
        OpenGL::GL
)

# El resto de librerias del sistema que GLFW necesita cambian segun el
# sistema operativo.
if(APPLE)
    target_link_libraries(galeria
        PRIVATE
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
elseif(WIN32)
    target_link_libraries(galeria PRIVATE opengl32 gdi32)
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(galeria PRIVATE X11 dl pthread)
endif()

# Avisos utiles, no obligatorios.
target_compile_options(galeria PRIVATE -Wall -Wextra)

message(STATUS "OpenGL encontrado: ${OPENGL_gl_LIBRARY}")
