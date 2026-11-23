// ============================================================================
// Shader.h — clase pequeña para manejar un programa de shaders (vertex +
// fragment) de OpenGL. Se encarga de leer los archivos .vert/.frag, pedirle
// a OpenGL que los compile y los linkee, y dejar listo el program id para
// usarlo con glUseProgram(). Tambien tiene metodos para mandar matrices y
// colores como uniforms, para no repetir glGetUniformLocation en todos
// lados.
// ============================================================================
#pragma once

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class Shader {
public:
    GLuint id = 0;

    // Carga y compila un shader a partir de dos archivos de texto: uno con
    // el codigo del vertex shader y otro con el del fragment shader.
    Shader(const std::string& rutaVertex, const std::string& rutaFragment) {
        std::string vertexSrc = leerArchivo(rutaVertex);
        std::string fragmentSrc = leerArchivo(rutaFragment);

        GLuint vs = compilar(GL_VERTEX_SHADER, vertexSrc, rutaVertex);
        GLuint fs = compilar(GL_FRAGMENT_SHADER, fragmentSrc, rutaFragment);

        id = glCreateProgram();
        glAttachShader(id, vs);
        glAttachShader(id, fs);
        glLinkProgram(id);
        revisarLink(id, rutaVertex + " + " + rutaFragment);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void usar() const {
        glUseProgram(id);
    }

    void setMat4(const std::string& nombre, const glm::mat4& valor) const {
        GLint loc = glGetUniformLocation(id, nombre.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(valor));
    }

    void setVec3(const std::string& nombre, const glm::vec3& valor) const {
        GLint loc = glGetUniformLocation(id, nombre.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(valor));
    }

    void setInt(const std::string& nombre, int valor) const {
        GLint loc = glGetUniformLocation(id, nombre.c_str());
        glUniform1i(loc, valor);
    }

    GLint location(const std::string& nombre) const {
        return glGetUniformLocation(id, nombre.c_str());
    }

private:
    static std::string leerArchivo(const std::string& ruta) {
        std::ifstream archivo(ruta);
        if (!archivo.is_open()) {
            std::cout << "ERROR: no se pudo abrir el shader: " << ruta << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }

    static GLuint compilar(GLenum tipo, const std::string& codigo, const std::string& nombre) {
        GLuint shader = glCreateShader(tipo);
        const char* src = codigo.c_str();
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        int exito;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &exito);
        if (!exito) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Error compilando shader (" << nombre << "):\n" << infoLog << std::endl;
        }
        return shader;
    }

    static void revisarLink(GLuint program, const std::string& nombre) {
        int exito;
        glGetProgramiv(program, GL_LINK_STATUS, &exito);
        if (!exito) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "Error linkeando programa (" << nombre << "):\n" << infoLog << std::endl;
        }
    }
};
