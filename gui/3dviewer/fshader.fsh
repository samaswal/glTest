#version 330 core
in vec4 outColor;
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;
uniform sampler2D tex1;
uniform float rate;

void main() {
   FragColor = mix(texture(tex, vec2(TexCoord.x, TexCoord.y)), texture(tex1, TexCoord), rate);
}
