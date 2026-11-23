#version 330 core
in vec3 vWorldPos;
out vec4 FragColor;
uniform vec3 uColor;
void main(){
    float luz = clamp(0.78 + vWorldPos.y * 0.015, 0.55, 1.15);
    vec3 color = uColor * luz;
    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
