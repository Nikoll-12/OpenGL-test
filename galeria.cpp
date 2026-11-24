
//Quita los warnings de OpenGL marcado como obsoleto en mac//
#define GL_SILENCE_DEPRECATION
//Activa el codigo real de la libreria cgltf (lee los modelos)//
#define CGLTF_IMPLEMENTATION
#include "libs/cgltf.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
//Loader casero de funciones de OpenGL, reemplaza a GLAD//
#include "libs/gl_loader.h"
//Evita que GLFW incluya su propio OpenGL, ya lo trae el loader//
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Activa el codigo real de la libreria de imagenes//
#define STB_IMAGE_IMPLEMENTATION
#include "texturas/stb_image.h"

//Clase que carga y maneja los shaders//
#include "libs/Shader.h"

//Ajusta el area de dibujo cuando cambia el tamaño de la ventana//
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window; glViewport(0, 0, width, height);
}

//Posicion y direccion de la camara libre//
glm::vec3 camPos(0.0f, 2.2f, 11.0f);
glm::vec3 camFront(0.0f, 0.0f, -1.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f;
float camPitch = -10.0f;
float lastMouseX = 500.0f, lastMouseY = 350.0f;
bool primerMouse = true;

//Convierte yaw y pitch en el vector hacia donde mira la camara//
static void actualizarDireccionCamara() {
    glm::vec3 dir;
    dir.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    dir.y = sin(glm::radians(camPitch));
    dir.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    camFront = glm::normalize(dir);
}

//Mueve la vista de la camara segun el movimiento del mouse//
static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    if (primerMouse) { lastMouseX = (float)xpos; lastMouseY = (float)ypos; primerMouse = false; }
    float xoffset = (float)xpos - lastMouseX;
    float yoffset = lastMouseY - (float)ypos;
    lastMouseX = (float)xpos; lastMouseY = (float)ypos;

    float sensibilidad = 0.1f;
    camYaw += xoffset * sensibilidad;
    camPitch += yoffset * sensibilidad;
    if (camPitch > 89.0f) camPitch = 89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;
    actualizarDireccionCamara();
}

//Lee el teclado y mueve la camara (WASD, espacio, shift, flechas)//
static void procesarTeclado(GLFWwindow* window, float deltaTime) {
    float velocidad = 4.0f * deltaTime;
    glm::vec3 adelante = glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z));
    glm::vec3 derecha = glm::normalize(glm::cross(camFront, camUp));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPos += adelante * velocidad;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPos -= adelante * velocidad;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camPos -= derecha * velocidad;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camPos += derecha * velocidad;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camPos.y += velocidad;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camPos.y -= velocidad;

    //Las flechas tambien rotan la vista, igual que el mouse//
    float giro = 60.0f * deltaTime;
    bool cambioVista = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) { camYaw -= giro; cambioVista = true; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { camYaw += giro; cambioVista = true; }
    if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) { camPitch += giro; cambioVista = true; }
    if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) { camPitch -= giro; cambioVista = true; }
    if (cambioVista) {
        if (camPitch > 89.0f) camPitch = 89.0f;
        if (camPitch < -89.0f) camPitch = -89.0f;
        actualizarDireccionCamara();
    }
}

//Interpolacion lineal entre dos puntos, usada para la curva de Bezier//
static glm::vec3 lerp3(const glm::vec3& a, const glm::vec3& b, float t) {
    return a + (b - a) * t;
}
//Calcula un punto de la curva de Bezier cubica (De Casteljau)//
static glm::vec3 evaluarBezierCubica(const glm::vec3& p0, const glm::vec3& p1,
                                      const glm::vec3& p2, const glm::vec3& p3, float t) {
    glm::vec3 a = lerp3(p0, p1, t);
    glm::vec3 b = lerp3(p1, p2, t);
    glm::vec3 c = lerp3(p2, p3, t);
    glm::vec3 d = lerp3(a, b, t);
    glm::vec3 e = lerp3(b, c, t);
    return lerp3(d, e, t);
}


//Vertice con posicion y coordenada de textura (UV)//
struct VertexPU { GLfloat pos[3]; GLfloat uv[2]; };

//Carga una imagen y la sube a la GPU como textura//
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << "Textura cargada OK: " << path << " (" << width << "x" << height << ")" << std::endl;
    } else {
        std::cout << "ERROR: no se pudo cargar la textura: " << path << std::endl;
    }
    if (data) stbi_image_free(data);
    return textureID;
}

//Saca la carpeta de un path (para buscar texturas al lado del modelo)//
static std::string dirDe(const std::string& path) {
    size_t p = path.find_last_of('/');
    if (p == std::string::npos) return "";
    return path.substr(0, p + 1);
}

//Reusa una textura ya cargada en vez de cargarla de nuevo//
static GLuint loadTextureCached(const std::string& path, std::map<std::string, GLuint>& cache) {
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;
    GLuint id = loadTexture(path.c_str());
    cache[path] = id;
    return id;
}


//Una pieza del modelo ya subida a la GPU (VAO/VBO/EBO) con su textura o color//
struct GltfPart {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
    GLuint texture = 0;
    bool hasTexture = false;
    glm::vec3 colorFallback = glm::vec3(0.75f);
};

//Un modelo completo: todas sus piezas, mas datos del tamaño/centro//
struct GltfScene {
    std::vector<GltfPart> parts;
    glm::vec3 centro = glm::vec3(0.0f);
    float minY = 0.0f;
    float mayorLado = 1.0f;
};

//Lee un archivo .gltf y sube todas sus piezas a la GPU//
static bool loadGltfScene(const char* path, GltfScene& out, std::map<std::string, GLuint>& texCache,
                           const char* excluirNodo = nullptr) {
    cgltf_options options = {};
    cgltf_data* data = NULL;
    if (cgltf_parse_file(&options, path, &data) != cgltf_result_success) {
        std::cout << "ERROR: no se pudo abrir " << path << std::endl; return false;
    }
    if (cgltf_load_buffers(&options, data, path) != cgltf_result_success) {
        std::cout << "ERROR: no se pudieron cargar los buffers de " << path << std::endl;
        cgltf_free(data); return false;
    }

    std::string base = dirDe(path);
    glm::vec3 mn(1e30f), mx(-1e30f);

    //Recorre cada nodo (pedazo) del modelo//
    for (cgltf_size ni = 0; ni < data->nodes_count; ni++) {
        cgltf_node* nodo = &data->nodes[ni];
        //Se salta nodos sin malla o con animacion por huesos//
        if (nodo->mesh == NULL || nodo->skin != NULL) continue;
        if (excluirNodo != NULL && nodo->name != NULL && std::string(nodo->name) == excluirNodo) continue;

        float M[16];
        cgltf_node_transform_world(nodo, M);
        glm::mat4 nodeMat = glm::make_mat4(M);

        for (cgltf_size pi = 0; pi < nodo->mesh->primitives_count; pi++) {
            cgltf_primitive* prim = &nodo->mesh->primitives[pi];
            if (prim->type != cgltf_primitive_type_triangles || prim->indices == NULL) continue;
            const cgltf_accessor* pos = cgltf_find_accessor(prim, cgltf_attribute_type_position, 0);
            if (pos == NULL || pos->count == 0) continue;
            const cgltf_accessor* uv = cgltf_find_accessor(prim, cgltf_attribute_type_texcoord, 0);

            std::vector<float> rawPos(pos->count * 3);
            cgltf_accessor_unpack_floats(pos, rawPos.data(), rawPos.size());
            std::vector<float> rawUV;
            if (uv != NULL) {
                rawUV.resize(uv->count * 2);
                cgltf_accessor_unpack_floats(uv, rawUV.data(), rawUV.size());
            }

            //Pasa cada vertice al espacio del mundo y guarda su UV//
            std::vector<VertexPU> verts(pos->count);
            for (cgltf_size i = 0; i < pos->count; i++) {
                glm::vec4 p = nodeMat * glm::vec4(rawPos[i*3], rawPos[i*3+1], rawPos[i*3+2], 1.0f);
                verts[i].pos[0] = p.x; verts[i].pos[1] = p.y; verts[i].pos[2] = p.z;
                if (!rawUV.empty() && i < uv->count) { verts[i].uv[0] = rawUV[i*2]; verts[i].uv[1] = rawUV[i*2+1]; }
                else { verts[i].uv[0] = 0.0f; verts[i].uv[1] = 0.0f; }
                mn = glm::min(mn, glm::vec3(p));
                mx = glm::max(mx, glm::vec3(p));
            }

            std::vector<unsigned int> idx(prim->indices->count);
            cgltf_accessor_unpack_indices(prim->indices, idx.data(), sizeof(unsigned int), idx.size());

            //Sube el VAO, VBO y EBO de esta pieza a la GPU//
            GltfPart part;
            glGenVertexArrays(1, &part.vao);
            glGenBuffers(1, &part.vbo);
            glGenBuffers(1, &part.ebo);
            glBindVertexArray(part.vao);
            glBindBuffer(GL_ARRAY_BUFFER, part.vbo);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(VertexPU), verts.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPU), (void*)offsetof(VertexPU, pos));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPU), (void*)offsetof(VertexPU, uv));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
            part.indexCount = (GLsizei)idx.size();

            //Busca la textura o el color del material//
            cgltf_material* mat = prim->material;
            if (mat != NULL && mat->has_pbr_metallic_roughness) {
                auto& pbr = mat->pbr_metallic_roughness;
                part.colorFallback = glm::vec3(pbr.base_color_factor[0], pbr.base_color_factor[1], pbr.base_color_factor[2]);
                if (pbr.base_color_texture.texture != NULL &&
                    pbr.base_color_texture.texture->image != NULL &&
                    pbr.base_color_texture.texture->image->uri != NULL) {
                    std::string imgPath = base + pbr.base_color_texture.texture->image->uri;
                    part.texture = loadTextureCached(imgPath, texCache);
                    part.hasTexture = true;
                }
            }
            out.parts.push_back(part);
        }
    }

    if (out.parts.empty()) {
        std::cout << "ERROR: " << path << " no produjo geometria valida" << std::endl;
        cgltf_free(data); return false;
    }

    //Calcula centro, altura minima y tamaño de todo el modelo//
    out.centro = (mn + mx) * 0.5f;
    out.minY = mn.y;
    glm::vec3 ext = mx - mn;
    out.mayorLado = glm::max(ext.x, glm::max(ext.y, ext.z));
    if (out.mayorLado <= 0.0f) out.mayorLado = 1.0f;

    cgltf_free(data);
    std::cout << "Cargado " << path << ": " << out.parts.size() << " piezas, "
               << "bbox mayor lado = " << out.mayorLado << std::endl;
    return true;
}


//Dibuja todas las piezas de un modelo, eligiendo shader segun tenga textura o no//
static void dibujarGltfScene(const GltfScene& scene, const glm::mat4& model,
                              const Shader& shaderColor, const Shader& shaderTextura) {
    for (auto& part : scene.parts) {
        if (part.hasTexture) {
            shaderTextura.usar();
            shaderTextura.setMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, part.texture);
        } else {
            shaderColor.usar();
            shaderColor.setMat4("model", model);
            shaderColor.setVec3("uColor", part.colorFallback);
        }
        glBindVertexArray(part.vao);
        glDrawElements(GL_TRIANGLES, part.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

int main() {
    //Crea la ventana con un contexto de OpenGL 3.3 core//
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1100, 700, "Playa tropical - flamenco (Bezier)", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Pide las funciones de OpenGL con el loader casero//
    if (!cargarGL()) {
        std::cout << "No se pudieron cargar las funciones de OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }

    //Activa el mouse en modo camara y oculta el cursor//
    glfwFocusWindow(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    actualizarDireccionCamara();

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //Activa el test de profundidad para que lo de adelante tape lo de atras//
    glEnable(GL_DEPTH_TEST);

    //Un shader para piezas sin textura y otro para piezas con textura//
    Shader shaderColor("shaders/color.vert", "shaders/color.frag");
    Shader shaderTextura("shaders/textura.vert", "shaders/textura.frag");

    std::map<std::string, GLuint> texCache;

    //Carga la isla y la escala/ancla al tamaño del mundo//
    GltfScene isla;
    bool okIsla = loadGltfScene("modelos/isla/Tropical_Sketchfab.gltf", isla, texCache, "Skybox");
    const float ISLA_DIAMETRO = 22.0f; //Tamaño final de la isla en el mundo//
    float islaEscala = okIsla ? (ISLA_DIAMETRO / isla.mayorLado) : 1.0f;

    const float ISLA_SUELO_NATIVO = 0.1f; //Nivel de la arena en el modelo original//
    glm::vec3 islaAncla = okIsla ? glm::vec3(isla.centro.x, ISLA_SUELO_NATIVO, isla.centro.z) : glm::vec3(0.0f);
    glm::mat4 islaModel = glm::scale(glm::mat4(1.0f), glm::vec3(islaEscala))
                         * glm::translate(glm::mat4(1.0f), -islaAncla);

    //Carga el flamenco y lo escala a su tamaño//
    GltfScene flamenco;
    bool okFlamenco = loadGltfScene("modelos/flamenco/scene.gltf", flamenco, texCache);
    const float FLAMENCO_TAMANO = 1.6f;
    float flamencoEscala = okFlamenco ? (FLAMENCO_TAMANO / flamenco.mayorLado) : 1.0f;
    glm::vec3 flamencoAncla = okFlamenco ? flamenco.centro : glm::vec3(0.0f);

    //Carga la persona y la ubica parada junto a las palmeras//
    GltfScene persona;
    bool okPersona = loadGltfScene("modelos/persona/scene.gltf", persona, texCache);
    const float PERSONA_ALTURA = 1.75f;
    float personaEscala = okPersona ? (PERSONA_ALTURA / persona.mayorLado) : 1.0f;
    glm::vec3 personaAncla = okPersona ? glm::vec3(persona.centro.x, persona.minY, persona.centro.z) : glm::vec3(0.0f);

    glm::vec3 personaNativa(-0.6f, ISLA_SUELO_NATIVO, -0.4f);
    glm::vec3 personaPos = okIsla ? (personaNativa - islaAncla) * islaEscala
                                   : glm::vec3(3.0f, 0.0f, 6.0f);
    float personaAngulo = 0.0f;
    const float PERSONA_VELOCIDAD_GIRO = 35.0f; //Grados por segundo que gira//

    //Los 4 puntos de control de la curva de Bezier (vuelo del flamenco)//
    glm::vec3 bezierP0(-9.0f, 6.5f,  7.0f);
    glm::vec3 bezierP1(-9.0f, 8.0f, -7.0f);
    glm::vec3 bezierP2( 9.0f, 7.0f, -7.0f);
    glm::vec3 bezierP3( 9.0f, 6.5f,  7.0f);

    float vueloT = 0.0f;
    float vueloDir = 1.0f;
    const float VELOCIDAD_VUELO = 0.08f; //Que tan rapido recorre la curva//

    float lastFrame = 0.0f;

    //Loop principal: se repite una vez por cada frame//
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        procesarTeclado(window, deltaTime);
        //Unico limite de la camara: no bajar del piso//
        if (camPos.y < 0.3f) camPos.y = 0.3f;

        //Matrices de vista y proyeccion segun la camara actual//
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = height > 0 ? (float)width / (float)height : 1.0f;
        glm::mat4 projection = glm::perspective(glm::radians(65.0f), aspect, 0.05f, 200.0f);

        //Limpia la pantalla con el color del cielo//
        glClearColor(0.55f, 0.80f, 0.94f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Manda la camara a los dos shaders//
        shaderColor.usar();
        shaderColor.setMat4("view", view);
        shaderColor.setMat4("projection", projection);
        shaderTextura.usar();
        shaderTextura.setMat4("view", view);
        shaderTextura.setMat4("projection", projection);

        //Dibuja la isla, es fija y no se mueve//
        if (okIsla) dibujarGltfScene(isla, islaModel, shaderColor, shaderTextura);

        //Mueve el flamenco por la curva de Bezier (ida y vuelta)//
        if (okFlamenco) {
            vueloT += vueloDir * VELOCIDAD_VUELO * deltaTime;
            if (vueloT > 1.0f) { vueloT = 1.0f; vueloDir = -1.0f; }
            if (vueloT < 0.0f) { vueloT = 0.0f; vueloDir = 1.0f; }

            glm::vec3 posVuelo = evaluarBezierCubica(bezierP0, bezierP1, bezierP2, bezierP3, vueloT);
            //Calcula hacia donde mira el flamenco usando un punto mas adelante en la curva//
            float tAdelante = glm::clamp(vueloT + vueloDir * 0.01f, 0.0f, 1.0f);
            glm::vec3 puntoAdelante = evaluarBezierCubica(bezierP0, bezierP1, bezierP2, bezierP3, tAdelante);
            glm::vec3 dirVuelo = puntoAdelante - posVuelo;
            if (glm::length(dirVuelo) < 1e-5f) dirVuelo = glm::vec3(0.0f, 0.0f, -1.0f);
            dirVuelo = glm::normalize(dirVuelo);
            float anguloVuelo = glm::degrees(atan2(dirVuelo.x, dirVuelo.z));

            glm::mat4 flamencoModel = glm::translate(glm::mat4(1.0f), posVuelo)
                                     * glm::rotate(glm::mat4(1.0f), glm::radians(anguloVuelo), glm::vec3(0,1,0))
                                     * glm::scale(glm::mat4(1.0f), glm::vec3(flamencoEscala))
                                     * glm::translate(glm::mat4(1.0f), -flamencoAncla);
            dibujarGltfScene(flamenco, flamencoModel, shaderColor, shaderTextura);
        }

        //Gira la persona sobre su propio eje//
        if (okPersona) {
            personaAngulo += PERSONA_VELOCIDAD_GIRO * deltaTime;
            glm::mat4 personaModel = glm::translate(glm::mat4(1.0f), personaPos)
                                     * glm::rotate(glm::mat4(1.0f), glm::radians(personaAngulo), glm::vec3(0,1,0))
                                     * glm::scale(glm::mat4(1.0f), glm::vec3(personaEscala))
                                     * glm::translate(glm::mat4(1.0f), -personaAncla);
            dibujarGltfScene(persona, personaModel, shaderColor, shaderTextura);
        }

        //Muestra el frame y revisa teclado/mouse/ventana//
        glfwSwapBuffers(window);
        glfwPollEvents();

        //Cierra la ventana si se presiona ESC//
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    }

    glfwTerminate();
    return 0;
}
