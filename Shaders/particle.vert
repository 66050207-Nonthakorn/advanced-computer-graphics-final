#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aPacked;  // x = brightness, y = corner v, z = alpha
layout (location = 2) in vec2 aUv;      // corner uv

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float brightness;
out vec2  uv;
out float alpha;

void main() {
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    
    brightness = aPacked.x;
    uv = vec2(aUv.x, aPacked.y);
    alpha = aPacked.z;
}
