#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;

void main() {
    vec3 worldPos = vec3(model[3]);

    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);

    float scaleX = length(vec3(model[0]));
    float scaleY = length(vec3(model[1]));

    vec3 vertWorld = worldPos
        + right * aPosition.x * scaleX
        + up    * aPosition.y * scaleY;

    gl_Position = projection * view * vec4(vertWorld, 1.0);
    uv = aUv;
}
