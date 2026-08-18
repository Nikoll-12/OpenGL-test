
#define GL_SILENCE_DEPRECATION 
#include <iostream> 
#include <OpenGL/gl3.h>//macOS marca toda la API GL como deprecada desde 10.14
#include <GLFW/glfw3.h>
#include <vector> //SIrven para construir la geometría de la medusa// 
#include <cmath> //Funciones necesarias para las ondas de las patas de la medusa//

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}
//Inicializa el shader de la posicion de los vertices del triangulo// 
const char*vertexShaderSource="#version 330 core\n"
"layout (location=0) in vec3 apos;\n" //Posicion es un vector de 3 variables// 
"void main()\n"
"{\n"
"gl_Position=vec4(apos.x,apos.y,apos.z,1.0);\n" //Por que es vec4??? 
"}\0";
//Fragment shader recibe el color como uniform porque tenemos más elementos y reutiliza en ojos, cara y tentaculos// 
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor; //Casilla de memoria de c++, para almacear los colores// 
void main()
{
    FragColor = uColor; 
}
)";
//Sin el uColor, tocaría usar un shader para cada color que conforma la medusa//

//CAMBIO DEL SHADER DEL COLOR// 
/*const char *fragmentShaderSource="#version 330 core \n" //Inicializa el shader de color del triangulo// 
"out vec4 FragColor;\n"
"void main ()\n"
"{\n"
"FragColor=vec4(1.0f,0.85f,0.9f,1.0f);\n" //Shader de color del triangulo// 
"}\n\0";*/ 

int main ()//Inicaliza C++// 
//Los tentanculos de la medusa de construyen como pequeños triangulos para que sean de mas de 1px y la onda con SEN Y COS// 
//Tres Vaos separados para tocar cada uno de los elementos sin que se afenten entre si// 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLfloat vertices[] = { //Coordenadas del triangulo// 
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
GLFWwindow*window= glfwCreateWindow(500,500,"Medusa",NULL,NULL);
   if (window==NULL)
   {
    std::cout<<"Failed to create GLW WINDOW"<< std::endl;
    glfwTerminate();
    return -1;
   }
   //Formar la ventana del programa// 
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
    //Se conecta con ucolor para que reciba colores// 
    GLint colorLoc=glGetUniformLocation(ShaderProgram,"uColor");

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

   //Creación de los ojos de la medusa// 
   //Genero un VBO Y UN VAO aparte para unicamemnte tocar los ojos// 
   int scleraSegments=24; 
   int pupilSegments=16; 
   //cx,cy, radio, semgmentos//
   float eyes [4][4]={ 
              { -0.12f, 0.05f, 0.09f, (float)scleraSegments }, // esclera izquierda
        {  0.12f, 0.05f, 0.09f, (float)scleraSegments }, // esclera derecha
        { -0.10f, 0.05f, 0.04f, (float)pupilSegments  }, // pupila izquierda
        {  0.14f, 0.05f, 0.04f, (float)pupilSegments  }  // pupila derecha

   };
std::vector<GLfloat> eyesVertexs;
int eyesstart[4]; 
int eyescount[4]; 
//Recorro el array de los ojos para generar los vértices// 
for (int  i=0;i<4;i++){ 

   float cx=eyes[i][0]; 
   float cy= eyes[i][1]; 
   float radio=eyes[i][2]; 
   int segments= (int)eyes[i][3]; 
    
   eyesstart[i]=(int)(eyesVertexs.size()/3); //
   eyesVertexs.push_back(cx); 
   eyesVertexs.push_back(cy);
   eyesVertexs.push_back(0.0f); 
   
   for (int j=0;j<=segments;j++){ 
      float radiox=2.0f*3.14159f*float(j)/float(segments); 
      eyesVertexs.push_back(cx+radio*cosf(radiox)); 
      eyesVertexs.push_back(cy+radio*sinf(radiox)); 
      eyesVertexs.push_back((0.0f));
   }
   eyescount[i]=(int)(eyesVertexs.size()/3)-eyesstart[i];
}
GLuint eyesVAO, eyesVBO;
glGenVertexArrays(1,&eyesVAO);
glGenBuffers(1,&eyesVBO); 

glBindVertexArray(eyesVAO); 
glBindBuffer(GL_ARRAY_BUFFER,eyesVBO); 
glBufferData(GL_ARRAY_BUFFER,eyesVertexs.size()*sizeof(GLfloat),eyesVertexs.data(),GL_STATIC_DRAW); 
glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0); 
glEnableVertexAttribArray(0); 

glBindBuffer(GL_ARRAY_BUFFER,0); 
glBindVertexArray(0); 

//La boca: un rectangulo simple, EBO sin trigonometria ni loops//
GLfloat mouthVertices[] = {
   -0.08f, -0.13f, 0.0f,  //0: arriba-izquierda
    0.08f, -0.13f, 0.0f,  //1: arriba-derecha
    0.08f, -0.19f, 0.0f,  //2: abajo-derecha
   -0.08f, -0.19f, 0.0f   //3: abajo-izquierda
};
GLuint mouthIndices[] = {
   0,1,2,  //primer triangulo
   0,2,3   //segundo triangulo, reusa el 0 y el 2
};

GLuint mouthVAO, mouthVBO, mouthEBO;
glGenVertexArrays(1,&mouthVAO);
glGenBuffers(1,&mouthVBO);
glGenBuffers(1,&mouthEBO);

glBindVertexArray(mouthVAO);
glBindBuffer(GL_ARRAY_BUFFER,mouthVBO);
glBufferData(GL_ARRAY_BUFFER,sizeof(mouthVertices),mouthVertices,GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mouthEBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(mouthIndices),mouthIndices,GL_STATIC_DRAW);
glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER,0);
glBindVertexArray(0);
 
glClearColor(0.55f,0.80f,0.85f,1.0f); 
glClear(GL_COLOR_BUFFER_BIT); 
glfwSwapBuffers(window); 

while (!glfwWindowShouldClose(window)){
   glClearColor(0.0f,0.400f,0.60f,1.0f);//Color del fondo// 

   glClear(GL_COLOR_BUFFER_BIT);
   glUseProgram(ShaderProgram); 
   
   glUniform4f(colorLoc, 1.0f,0.85f,0.9f,1.0f); //Color del cuerpo// 
   glBindVertexArray(VAO); 
   glDrawArrays(GL_TRIANGLES,0,3); 
//Primero las esferas blancas y luego la pupilas negras// 
glBindVertexArray(eyesVAO); 
glUniform4f(colorLoc,1.0f,1.0f,1.0f,1.0f); 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[0],eyescount[0]); 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[1],eyescount[1]);
glUniform4f(colorLoc,0.05f,0.05f,0.05f,1.0f); 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[2],eyescount[2]); 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[3],eyescount[3]); 
//La boca, con glDrawElements//
glBindVertexArray(mouthVAO);
glUniform4f(colorLoc,0.75f,0.25f,0.35f,1.0f);
glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);


glfwSwapBuffers(window);    
glfwPollEvents(); 
}
glDeleteVertexArrays(1,&VAO);
glDeleteBuffers(1,&VBO);
glDeleteVertexArrays(1,&eyesVAO);
glDeleteBuffers(1,&eyesVBO);
glDeleteVertexArrays(1,&mouthVAO);
glDeleteBuffers(1,&mouthVBO);
glDeleteBuffers(1,&mouthEBO);

glDeleteProgram(ShaderProgram);

   glfwDestroyWindow(window); 
glfwTerminate();
    return 0;
}

