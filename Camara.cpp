#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ---------- Shader de los triángulos ----------
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location=0) in vec3 apos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(apos, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 uColor;\n"
"void main()\n"
"{\n"
"FragColor = uColor;\n"
"}\0";

// ---------- Shader de las líneas (ejes) ----------
const char* lineVertexShaderSource =
"#version 330 core\n"
"layout (location=0) in vec3 aPos;\n"
"layout (location=1) in vec4 aColor;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec4 vColor;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * vec4(aPos, 1.0);\n"
"vColor = aColor;\n"
"}\0";

const char* lineFragmentShaderSource =
"#version 330 core\n"
"in vec4 vColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vColor;\n"
"}\0";

struct LineVertex {
    GLfloat pos[3];
    GLfloat color[4];
};

static void checkShaderCompile(GLuint shader, const char* name) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Error compilando shader (" << name << "):\n" << infoLog << std::endl;
    }
}
static void checkProgramLink(GLuint program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Error linkeando programa:\n" << infoLog << std::endl;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    static const LineVertex kDefaultOrigin[] = {
        { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    GLFWwindow* window = glfwCreateWindow(500, 500, "View matrix", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---- shader de triángulos ----
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompile(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompile(fragmentShader, "FRAGMENT");

    GLuint ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);
    checkProgramLink(ShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint colorLoc = glGetUniformLocation(ShaderProgram, "uColor");
    GLint modelLoc = glGetUniformLocation(ShaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(ShaderProgram, "view");
    GLint projLoc = glGetUniformLocation(ShaderProgram, "projection");

    // ---- shader de líneas ----
    GLuint lineVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(lineVS, 1, &lineVertexShaderSource, NULL);
    glCompileShader(lineVS);
    checkShaderCompile(lineVS, "LINE VERTEX");

    GLuint lineFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lineFS, 1, &lineFragmentShaderSource, NULL);
    glCompileShader(lineFS);
    checkShaderCompile(lineFS, "LINE FRAGMENT");

    GLuint lineShaderProgram = glCreateProgram();
    glAttachShader(lineShaderProgram, lineVS);
    glAttachShader(lineShaderProgram, lineFS);
    glLinkProgram(lineShaderProgram);
    checkProgramLink(lineShaderProgram);

    glDeleteShader(lineVS);
    glDeleteShader(lineFS);

    GLint lineViewLoc = glGetUniformLocation(lineShaderProgram, "view");
    GLint lineProjLoc = glGetUniformLocation(lineShaderProgram, "projection");

    // ---- VAO/VBO del triángulo ----
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ---- VAO/VBO de las líneas (ejes) ----
    GLuint VAO_lines, VBO_lines;
    glGenVertexArrays(1, &VAO_lines);
    glGenBuffers(1, &VBO_lines);

    glBindVertexArray(VAO_lines);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_lines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kDefaultOrigin), kDefaultOrigin, GL_STATIC_DRAW);
    GLsizei strideLine = sizeof(LineVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideLine, (void*)offsetof(LineVertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, strideLine, (void*)offsetof(LineVertex, color));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // proyección recalculada cada frame
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = height > 0 ? (float)width / (float)height : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);

        // ---- cámara orbitando (esto es lo nuevo) ----
        float camAngle = (float)glfwGetTime() * 0.5f;   // velocidad de la órbita
        float radius = 5.0f;                             // qué tan lejos orbita
        float camX = sin(camAngle) * radius;
        float camZ = cos(camAngle) * radius;

        glm::mat4 view = glm::lookAt(
            glm::vec3(camX, 0.0f, camZ),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // ---- dibujar los ejes primero ----
        glUseProgram(lineShaderProgram);
        glUniformMatrix4fv(lineViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(lineProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO_lines);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        // ---- dibujar los 3 triángulos (sin rotación propia; ya la aporta la cámara) ----
        glUseProgram(ShaderProgram);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(VAO);

        // Grande (más lejano)
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(-1.0f, 0.0f, -3.0f));
        model1 = glm::scale(model1, glm::vec3(1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
        glUniform4f(colorLoc, 1.0f, 0.75f, 0.85f, 0.6f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Mediano
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, -1.5f));
        model2 = glm::scale(model2, glm::vec3(0.6f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
        glUniform4f(colorLoc, 0.7f, 0.85f, 1.0f, 0.8f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Pequeño (más cercano)
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(1.0f, 0.0f, 0.0f));
        model3 = glm::scale(model3, glm::vec3(0.35f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
        glUniform4f(colorLoc, 0.9f, 0.6f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

