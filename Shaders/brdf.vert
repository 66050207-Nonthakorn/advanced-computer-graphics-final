#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aUv;

out vec2 uv;

void main() {
    uv = aUv;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}
