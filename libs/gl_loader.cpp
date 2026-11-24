// Definicion de los punteros declarados en gl_loader.h, y la funcion que los
// resuelve con glfwGetProcAddress (funciona igual en macOS, Windows y Linux).
#include "gl_loader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
void (*glClear)(GLbitfield) = nullptr;
void (*glEnable)(GLenum) = nullptr;
void (*glViewport)(GLint, GLint, GLsizei, GLsizei) = nullptr;

void (*glGenVertexArrays)(GLsizei, GLuint*) = nullptr;
void (*glBindVertexArray)(GLuint) = nullptr;
void (*glGenBuffers)(GLsizei, GLuint*) = nullptr;
void (*glBindBuffer)(GLenum, GLuint) = nullptr;
void (*glBufferData)(GLenum, GLsizeiptr, const void*, GLenum) = nullptr;
void (*glEnableVertexAttribArray)(GLuint) = nullptr;
void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*) = nullptr;
void (*glDrawElements)(GLenum, GLsizei, GLenum, const void*) = nullptr;

void (*glGenTextures)(GLsizei, GLuint*) = nullptr;
void (*glBindTexture)(GLenum, GLuint) = nullptr;
void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*) = nullptr;
void (*glTexParameteri)(GLenum, GLenum, GLint) = nullptr;
void (*glGenerateMipmap)(GLenum) = nullptr;
void (*glActiveTexture)(GLenum) = nullptr;

GLuint (*glCreateShader)(GLenum) = nullptr;
void (*glShaderSource)(GLuint, GLsizei, const GLchar* const*, const GLint*) = nullptr;
void (*glCompileShader)(GLuint) = nullptr;
void (*glGetShaderiv)(GLuint, GLenum, GLint*) = nullptr;
void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) = nullptr;
void (*glDeleteShader)(GLuint) = nullptr;

GLuint (*glCreateProgram)(void) = nullptr;
void (*glAttachShader)(GLuint, GLuint) = nullptr;
void (*glLinkProgram)(GLuint) = nullptr;
void (*glGetProgramiv)(GLuint, GLenum, GLint*) = nullptr;
void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) = nullptr;
void (*glUseProgram)(GLuint) = nullptr;

GLint (*glGetUniformLocation)(GLuint, const GLchar*) = nullptr;
void (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*) = nullptr;
void (*glUniform3fv)(GLint, GLsizei, const GLfloat*) = nullptr;
void (*glUniform1i)(GLint, GLint) = nullptr;

namespace {
    template <typename T>
    bool cargar(T& puntero, const char* nombre) {
        puntero = reinterpret_cast<T>(glfwGetProcAddress(nombre));
        return puntero != nullptr;
    }
}

bool cargarGL() {
    bool ok = true;
    ok &= cargar(glClearColor, "glClearColor");
    ok &= cargar(glClear, "glClear");
    ok &= cargar(glEnable, "glEnable");
    ok &= cargar(glViewport, "glViewport");

    ok &= cargar(glGenVertexArrays, "glGenVertexArrays");
    ok &= cargar(glBindVertexArray, "glBindVertexArray");
    ok &= cargar(glGenBuffers, "glGenBuffers");
    ok &= cargar(glBindBuffer, "glBindBuffer");
    ok &= cargar(glBufferData, "glBufferData");
    ok &= cargar(glEnableVertexAttribArray, "glEnableVertexAttribArray");
    ok &= cargar(glVertexAttribPointer, "glVertexAttribPointer");
    ok &= cargar(glDrawElements, "glDrawElements");

    ok &= cargar(glGenTextures, "glGenTextures");
    ok &= cargar(glBindTexture, "glBindTexture");
    ok &= cargar(glTexImage2D, "glTexImage2D");
    ok &= cargar(glTexParameteri, "glTexParameteri");
    ok &= cargar(glGenerateMipmap, "glGenerateMipmap");
    ok &= cargar(glActiveTexture, "glActiveTexture");

    ok &= cargar(glCreateShader, "glCreateShader");
    ok &= cargar(glShaderSource, "glShaderSource");
    ok &= cargar(glCompileShader, "glCompileShader");
    ok &= cargar(glGetShaderiv, "glGetShaderiv");
    ok &= cargar(glGetShaderInfoLog, "glGetShaderInfoLog");
    ok &= cargar(glDeleteShader, "glDeleteShader");

    ok &= cargar(glCreateProgram, "glCreateProgram");
    ok &= cargar(glAttachShader, "glAttachShader");
    ok &= cargar(glLinkProgram, "glLinkProgram");
    ok &= cargar(glGetProgramiv, "glGetProgramiv");
    ok &= cargar(glGetProgramInfoLog, "glGetProgramInfoLog");
    ok &= cargar(glUseProgram, "glUseProgram");

    ok &= cargar(glGetUniformLocation, "glGetUniformLocation");
    ok &= cargar(glUniformMatrix4fv, "glUniformMatrix4fv");
    ok &= cargar(glUniform3fv, "glUniform3fv");
    ok &= cargar(glUniform1i, "glUniform1i");

    return ok;
}
