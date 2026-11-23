#version 330 core
in vec2 vUV;
in vec3 vWorldPos;
out vec4 FragColor;
uniform sampler2D uTex;
void main(){
    float luz = clamp(0.78 + vWorldPos.y * 0.015, 0.55, 1.15);
    vec4 tex = texture(uTex, vUV);
    if (tex.a < 0.05) discard;
    FragColor = vec4(clamp(tex.rgb * luz, 0.0, 1.0), tex.a);
}
