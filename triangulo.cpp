
#define GL_SILENCE_DEPRECATION 
#include <iostream> 
#include <OpenGL/gl3.h>//macOS marca toda la API GL como deprecada desde 10.14
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}

const char*vertexShaderSource="#version 330 core\n"
"layout (location=0) in vec3 apos;\n"
"void main()\n"
"{\n"
"gl_Position=vec4(apos.x,apos.y,apos.z,1.0);\n"
"}\0";
const char *fragmentShaderSource="#version 330 core \n"
"out vec4 FragColor;\n"
"void main ()\n"
"{\n"
"FragColor=vec4(1.0f,0.5f,0.2f,1.0f);\n"
"}\n\0";

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
GLFWwindow*window= glfwCreateWindow(500,500,"Triangulo",NULL,NULL);
   if (window==NULL)
   {
    std::cout<<"Failed to create GLW WINDOW"<< std::endl;
    glfwTerminate();
    return -1;
   }
   glfwMakeContextCurrent(window);
   int fbWidth, fbHeight;
   glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
   glViewport(0,0,fbWidth,fbHeight);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   GLuint vertexShader=glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
   glCompileShader(vertexShader);
   GLuint fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
   glCompileShader(fragmentShader);
   GLuint ShaderProgram=glCreateProgram();

   glAttachShader(ShaderProgram,vertexShader);
    glAttachShader(ShaderProgram,fragmentShader);
    glLinkProgram(ShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

GLuint VAO,VBO;

glGenVertexArrays(1,&VAO);
glGenBuffers(1,&VBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER,VBO);
glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER,0);
   glBindVertexArray(0);

      glClearColor(1.0f,0.85f,0.9f,1.0f);
glClear(GL_COLOR_BUFFER_BIT);
glfwSwapBuffers(window); 

   while (!glfwWindowShouldClose(window)){
      glClearColor(1.0f,0.85f,0.9f,1.0f);
glClear(GL_COLOR_BUFFER_BIT);

glUseProgram(ShaderProgram);
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES,0,3); 

glfwSwapBuffers(window);    
glfwPollEvents(); 
}
glDeleteVertexArrays(1,&VAO);
glDeleteBuffers(1,&VBO);
glDeleteProgram(ShaderProgram);

   glfwDestroyWindow(window); 
glfwTerminate();
    return 0;
}

