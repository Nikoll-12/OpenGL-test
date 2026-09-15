
#define GL_SILENCE_DEPRECATION //Evita que se muestren advertencias de OpenGl es uan tecnología antigua 
#include <iostream> //Permite mostrar mensajes en consola// 
#include <OpenGL/gl3.h>//Contiene las funciones principales de openGL//
#include <GLFW/glfw3.h>///Crear la ventana y recibir los eventos de teclado y mantiene abierto el programa// 
#include <vector> //permite guardar una cantidad variable de los vertices (ojos)// 
#include <cmath> //Funciones necesarias para los circulos//

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height); //Utiliza el área para que ocupe la ventana//  
}
//Inicializa el shader de la posicion de los vertices del triangulo// 
const char*vertexShaderSource="#version 330 core\n"
"layout (location=0) in vec3 apos;\n" //Posicion es un vector de 3 variables// 
"void main()\n"
"{\n"
"gl_Position=vec4(apos.x,apos.y,apos.z,1.0);\n" //Posicion y el 4 es un valor que exige// 
"}\0";
//Fragment shader recibe el color como uniform porque tenemos más elementos y reutiliza en ojos, cara y tentaculos// 
//Gracias a Ucolor reutilizamos el mismos shader para cada uno de los pixeles// 
//fragment shader decide el color de cada pixel CAMBIANDO EL COLOR DESDE C++ // 
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
//Tenemos un buffer separado 
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
    glfwInit();//  Arranca la librería de memoria GLFW//
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);//Pide contexto de openGl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);//Pide el decimal de la versión// 
    //Funciones modernas de OpenGl//
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//Permite usar las funciones modernas de OpenGl// 
    GLfloat vertices[] = { //Coordenadas del triangulo// 
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
GLFWwindow*window= glfwCreateWindow(500,500,"Medusa",NULL,NULL);
   if (window==NULL) //el atributo para la ventana en el computador// 
   {
    std::cout<<"Failed to create GLW WINDOW"<< std::endl;//Si algo falla dentro del programa// 
    glfwTerminate();
    return -1;//En caso de que no se pueda abrir la ventana por algun error dentro del código// 
   }
   //Formar la ventana del programa// 
   glfwMakeContextCurrent(window);//Activa el contexto de la ventana como el actual// 
   int fbWidth, fbHeight;//El alto y el ancho de la ventana//
   glfwGetFramebufferSize(window, &fbWidth, &fbHeight);//Obtiene el tamaño de la ventana// 
   glViewport(0,0,fbWidth,fbHeight);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//Actualiza el tamaño del objeto cuando cambiamos la ventana de tamañ0//

   GLuint vertexShader=glCreateShader(GL_VERTEX_SHADER);//Shader encargado de crear las posiciones// 
   glShaderSource(vertexShader,1,&vertexShaderSource,NULL);//Lleva el código fuente del shader a OPENGL// 
   glCompileShader(vertexShader);//CONVIERTE Tarjeta gráfica// 
   GLuint fragmentShader=glCreateShader(GL_FRAGMENT_SHADER); //Programa que reune todos los shader// 
   glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
   glCompileShader(fragmentShader);
   GLuint ShaderProgram=glCreateProgram();//Espacio vacío para unir vertex shader y fragment shader  (Información))// 

   glAttachShader(ShaderProgram,vertexShader);//Une los shader de color y de posición// 
    glAttachShader(ShaderProgram,fragmentShader);
    glLinkProgram(ShaderProgram);//Finaliza la unión para que trabajen juntos// 

    glDeleteShader(vertexShader);//Se borra de la memoria para que no quede guardado y no pese// 
    glDeleteShader(fragmentShader);
    //Se conecta con ucolor para que reciba colores// 
    //Ubicacion//
    GLint colorLoc=glGetUniformLocation(ShaderProgram,"uColor");

GLuint VAO,VBO;
//VAO organiza los datos de la figura// 
//VBO memoria en tarjeta gráfica// 

glGenVertexArrays(1,&VAO);
glGenBuffers(1,&VBO);

glBindVertexArray(VAO);//Activa el VAO que queremos utilizar// 

glBindBuffer(GL_ARRAY_BUFFER,VBO); //Copia los vertices que están en el bufffer// 
glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);//Los dibuja// 

glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);//Como organizar las coordenas// 
glEnableVertexAttribArray(0);//Activida la posicion 0 dentro de la matriz para que pueda así mismo utilizar todos los vertices// 

   glBindBuffer(GL_ARRAY_BUFFER,0);
   glBindVertexArray(0);

   //Creación de los ojos de la medusa// 
   //los ojos tienen una estructura repetitiva. Un ciclo recorre cada ojo y otro genera todos los puntos necesarios
   //  para dibujar cada círculo sin escribir coordenadas manualmente."

   //Genero un VBO Y UN VAO aparte para unicamemnte tocar los ojos// 
   int scleraSegments=24; //Cantidad de divisiones utlizadas para  crear el circulo//
   int pupilSegments=16; //Segmentos//
   //cx,cy, radio, semgmentos//
   float eyes [4][4]={ 
              { -0.12f, 0.05f, 0.09f, (float)scleraSegments }, // esclera izquierda
        {  0.12f, 0.05f, 0.09f, (float)scleraSegments }, // esclera derecha
        { -0.10f, 0.05f, 0.04f, (float)pupilSegments  }, // pupila izquierda
        {  0.14f, 0.05f, 0.04f, (float)pupilSegments  }  // pupila derecha

   };
std::vector<GLfloat> eyesVertexs;//Lista vacía para guardar las coordenadas de los ojos 
int eyesstart[4]; 
int eyescount[4]; 
//Recorro el array de los ojos para generar los vértices y construir los círculos// 
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
      //Calcula el angulo de cada punto alrededor del circulo// 
      float radiox=2.0f*3.14159f*float(j)/float(segments); 
      eyesVertexs.push_back(cx+radio*cosf(radiox));//Calcula la posición horizontal//  
      eyesVertexs.push_back(cy+radio*sinf(radiox)); //Calcula la posicion vertical// 
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


//Mantiene el programa funcionando hasta que el usuario cierre la ventana// 
while (!glfwWindowShouldClose(window)){
   glClearColor(0.0f,0.400f,0.60f,1.0f);//Color del fondo// 

   glClear(GL_COLOR_BUFFER_BIT);//Borra el dibujo anterior de la memoria para que se pueda volver a dibujar// 
   glUseProgram(ShaderProgram); 
   
   glUniform4f(colorLoc, 1.0f,0.85f,0.9f,1.0f); //Color del cuerpo// 
   glBindVertexArray(VAO); 
   glDrawArrays(GL_TRIANGLES,0,3); 
//Primero las esferas blancas y luego la pupilas negras// 
glBindVertexArray(eyesVAO); 
glUniform4f(colorLoc,1.0f,1.0f,1.0f,1.0f);//Encia el color rosado al Shader Uniform//  
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[0],eyescount[0]); //Se encarga de posicionar el triangle_fan es decir todos con el mismo punto de origen// 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[1],eyescount[1]);
glUniform4f(colorLoc,0.05f,0.05f,0.05f,1.0f); 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[2],eyescount[2]); //Dibuja el triangulo usando los vertices de posicion// 
glDrawArrays(GL_TRIANGLE_FAN,eyesstart[3],eyescount[3]); //Dibuja circulos unidos por el mismo vertice central// 
glUniform4f(colorLoc,0.75f,0.25f,0.35f,1.0f);
glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);


glfwSwapBuffers(window); //Muestra en pantalla todo lo qie dibujamos//    
glfwPollEvents(); //Revisa como cerrar la ventana o moverla// 
}
//Libera la memoría del programa// 
glDeleteVertexArrays(1,&VAO);
glDeleteBuffers(1,&VBO);
glDeleteVertexArrays(1,&eyesVAO);
glDeleteBuffers(1,&eyesVBO);


glDeleteProgram(ShaderProgram);

   glfwDestroyWindow(window); 
glfwTerminate();
    return 0;
}

