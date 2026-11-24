// ============================================================================
// gl_loader.h — mini reemplazo de GLAD hecho a mano, solo con las funciones y
// constantes de OpenGL que este proyecto usa. En macOS no hace falta (el
// sistema ya expone los punteros via <OpenGL/gl3.h>), pero en Windows/Linux
// las funciones de OpenGL moderno (3.3+) hay que pedirlas en tiempo de
// ejecucion. Para eso se usa glfwGetProcAddress, que GLFW ya trae y funciona
// igual en los tres sistemas, sin tener que instalar GLAD por separado.
//
// cargarGL() se llama una sola vez, justo despues de glfwMakeContextCurrent.
// ============================================================================
#pragma once

#include <cstddef>
#include <cstdint>

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef double GLdouble;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

#define GL_FALSE 0
#define GL_TRUE 1

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_DEPTH_TEST 0x0B71

#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_BYTE 0x1401
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_LINEAR 0x2601
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_REPEAT 0x2901
#define GL_RGB 0x1907
#define GL_RGBA 0x1908

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

// ---- Punteros a funciones, resueltos en tiempo de ejecucion -------------
extern void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
extern void (*glClear)(GLbitfield);
extern void (*glEnable)(GLenum);
extern void (*glViewport)(GLint, GLint, GLsizei, GLsizei);

extern void (*glGenVertexArrays)(GLsizei, GLuint*);
extern void (*glBindVertexArray)(GLuint);
extern void (*glGenBuffers)(GLsizei, GLuint*);
extern void (*glBindBuffer)(GLenum, GLuint);
extern void (*glBufferData)(GLenum, GLsizeiptr, const void*, GLenum);
extern void (*glEnableVertexAttribArray)(GLuint);
extern void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
extern void (*glDrawElements)(GLenum, GLsizei, GLenum, const void*);

extern void (*glGenTextures)(GLsizei, GLuint*);
extern void (*glBindTexture)(GLenum, GLuint);
extern void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
extern void (*glTexParameteri)(GLenum, GLenum, GLint);
extern void (*glGenerateMipmap)(GLenum);
extern void (*glActiveTexture)(GLenum);

extern GLuint (*glCreateShader)(GLenum);
extern void (*glShaderSource)(GLuint, GLsizei, const GLchar* const*, const GLint*);
extern void (*glCompileShader)(GLuint);
extern void (*glGetShaderiv)(GLuint, GLenum, GLint*);
extern void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
extern void (*glDeleteShader)(GLuint);

extern GLuint (*glCreateProgram)(void);
extern void (*glAttachShader)(GLuint, GLuint);
extern void (*glLinkProgram)(GLuint);
extern void (*glGetProgramiv)(GLuint, GLenum, GLint*);
extern void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
extern void (*glUseProgram)(GLuint);

extern GLint (*glGetUniformLocation)(GLuint, const GLchar*);
extern void (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
extern void (*glUniform3fv)(GLint, GLsizei, const GLfloat*);
extern void (*glUniform1i)(GLint, GLint);

// Carga todos los punteros de arriba usando glfwGetProcAddress. Llamar una
// sola vez, despues de glfwMakeContextCurrent(window).
bool cargarGL();
