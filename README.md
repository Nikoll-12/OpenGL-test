Nikoll Alejandra Borja Mayorca - 6014390

# Playa tropical con OpenGL 

Proyecto de la materia de Computación Gráfica. La idea mi idea fue armar una escena 3D
con una isla, un flamenco y una persona. 

# ¿Qué hace el programa?

Se descargaron 3 modelos en formato **glTF**: una isla, un flamenco y una
persona. A cada uno le aplique un tema distinto visto en clase:

- **Isla**: es la base de todo. me encargue de cargar un
  modelo glTF (con varias texturas: arena, agua, rocas, palmeras) y meterlo
  en la escena como escenario.
- **Flamenco**: vuela en círculos sobre la isla siguiendo una curva de
  Bézier. Aquí también se puede usar la **cámara libre** para volar
  junto a él y verlo desde cualquier ángulo mientras hace su recorrido.
- **Persona**: está parada entre las palmeras y el tema aplicado es
   en el rotación gira sobre su propio eje todo el tiempo.
- **Cámara libre** Se usa para moverse
  por toda la escena y poder ver cada cosa (la isla, el vuelo del flamenco,
  el giro de la persona) desde donde uno quiera. Se mueve con mouse (mirar) y
  WASD (caminar), espacio/shift para subir/bajar, y las flechas también
  mueven la vista por si no se quiere usar el mouse.
- El único límite que tiene la cámara es el piso: no se puede atravesar el
  suelo, pero no hay techo ni paredes, se puede volar libre por encima de
  toda la isla.

## Arquitectura 

galeria.cpp
├── 1. Shaders (clase Shader en libs/Shader.h + archivos .vert/.frag en shaders/)
│   ├── shaders/color.vert + shaders/color.frag: para piezas sin textura
│   └── shaders/textura.vert + shaders/textura.frag: para piezas con imagen
├── 2. Cámara libre
│   ├── variables de posición/ángulos (yaw, pitch)
│   ├── callback del mouse (mueve la vista)
│   └── función de teclado (WASD, espacio/shift, flechas)
├── 3. Curva de Bézier (función matemática para el vuelo del flamenco)
├── 4. Loader de modelos glTF (usa la librería cgltf.h)
│   └── convierte cada "nodo" del modelo en geometría que OpenGL puede dibujar
└── 5. main()
    ├── crea la ventana (GLFW)
    ├── compila los shaders
    ├── carga los 3 modelos con el loader de glTF
    └── loop principal: lee teclado/mouse, mueve/rota lo que corresponde,
        y dibuja todo en cada frame

 Explicación paso a paso

**1. La ventana.** Lo primero que hace `main()` es crear la ventana con
GLFW, pedir un contexto de OpenGL 3.3 "core", y activar el detector de
profundidad (`GL_DEPTH_TEST`) para que las cosas se dibujen en el orden
correcto (que lo de adelante tape lo de atrás).

**2. Los shaders.** El código GLSL ya no está como texto dentro de
`galeria.cpp`: vive en archivos aparte dentro de `shaders/`
(`color.vert`, `color.frag`, `textura.vert`, `textura.frag`). Para
cargarlos hice una clase `Shader` (`libs/Shader.h`) que lee esos archivos,
le pide a OpenGL que los compile y los linkee, y deja métodos listos para
usar (`usar()`, `setMat4()`, `setVec3()`) en vez de repetir
`glGetUniformLocation` por todos lados. Hay dos shaders: uno de color plano
(para piezas sin textura) y otro de textura (para piezas con imagen). Los
dos usan una **iluminación falsa**: no hay luces ni normales de verdad
calculadas, solo un truco donde entre más alto (coordenada Y) está un
punto, un poco más iluminado se ve. Es suficiente para que la escena no se
vea toda plana, sin tener que montar un sistema de luces completo.

**3. La cámara libre.** Es un estilo primera persona clásico usando las : se guardan
dos ángulos, yaw (izquierda/derecha) y pitch (arriba/abajo), y con eso se
calcula hacia dónde está mirando la cámara en cada frame. El mouse cambia
esos ángulos (`mouse_callback`), y el teclado (WASD) mueve la posición hacia
donde se está mirando (`procesarTeclado`). Las flechas del teclado hacen lo
mismo que el mouse (rotar la vista), por si alguien prefiere no usar mouse.
El único límite que se le puso es que no puede bajar del nivel del piso
aparte de eso no tiene techo ni paredes, para poder ver
la isla, el flamenco y la persona desde cualquier lugar.

**4. El loader de glTF.** Los 3 modelos (isla, flamenco, persona) vienen en
formato `.gltf` + `.bin` + imágenes, que es un formato estándar para modelos
3D. Para leerlo usé una librería chiquita llamada **cgltf** (un solo archivo
`.h`), porque escribir un parser de glTF desde cero sería otro proyecto
aparte. Lo que hace mi código con esa librería es:
  - Recorrer cada "nodo" del archivo (cada pedazo del modelo: una roca, una
    hoja de palmera, etc.)
  - Sacar sus vértices y sus coordenadas de textura (UV)
  - Subir esa geometría a la GPU (VAO/VBO/EBO, la forma normal de dibujar en
    OpenGL moderno)
  - Revisar si ese pedazo tiene una imagen asignada (textura) o si solo usa
    un color, para saber con cuál de los dos shaders dibujarlo después

**5. La curva de Bézier (vuelo del flamenco).** Es la función
`evaluarBezierCubica`: le doy 4 puntos de control en 3D y un valor `t` entre
0 y 1, y me devuelve la posición en la curva en ese instante. En el loop
principal voy aumentando `t` poco a poco con el tiempo, y cuando llega al
final invierto la dirección para que vaya y vuelva en bucle. También calculo
un punto "un poco más adelante" en la curva para saber hacia dónde está
mirando el flamenco y rotarlo en esa dirección (si no, volaría siempre
apuntando para el mismo lado, se vería raro).

**6. La rotación (persona).** Cada frame le sumo un ángulo
fijo a una variable (`personaAngulo += velocidad * deltaTime`) y ese ángulo
se usa en la matriz de rotación sobre el eje Y cuando la dibujo. Así gira
sobre su propio eje sin parar.

**7. El loop principal.** Cada vuelta del loop (cada frame) hace lo mismo:
lee teclado/mouse, actualiza la cámara, calcula las matrices de vista y
proyección, limpia la pantalla, y dibuja los 3 modelos uno por uno con su
propia matriz de posición (la isla es fija, el flamenco se recalcula con la
curva de Bézier, la persona se recalcula con su ángulo de giro actual).



