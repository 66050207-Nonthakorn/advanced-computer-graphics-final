#version 330 core

in vec3 normal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 lightPosition = vec3(0, 1, 3);
uniform vec3 lightColor = vec3(1.0, 0.5, 0.2);
uniform vec3 objectColor;

void main() {
    float ambient = 0.1;

    vec3 N = normalize(normal);
    vec3 V = lightPosition - fragPosition;
    float diffuse = max(dot(N, V), 0.0);

    fragColor = vec4((ambient + diffuse) * objectColor * lightColor, 1.0);
}