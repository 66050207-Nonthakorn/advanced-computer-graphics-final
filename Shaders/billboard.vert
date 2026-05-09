#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;

void main() {
    // Extract world position from model matrix, ignore rotation/scale on orientation.
    // Then build a billboard model matrix that uses the camera's right/up axes.
    vec3 worldPos = vec3(model[3]);

    // Camera right and up in world space (columns of the inverse view = transpose of view's rotation rows)
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);

    // Scale from model matrix diagonal
    float scaleX = length(vec3(model[0]));
    float scaleY = length(vec3(model[1]));

    vec3 vertWorld = worldPos
        + right * aPosition.x * scaleX
        + up    * aPosition.y * scaleY;

    gl_Position = projection * view * vec4(vertWorld, 1.0);
    uv = aUv;
}
