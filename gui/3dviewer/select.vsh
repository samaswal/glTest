#version 330 core
layout(location = 0) in vec3 position;
out vec4 outColor;
uniform float r;
uniform float g;
uniform float b;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    outColor = vec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}
