#version 330 core

in vec3 normal;
in vec3 fragPosition;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform vec3 lightPosition = vec3(0, 2, 3);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 V = normalize(viewPosition - fragPosition);

    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    float diffuseStrength = 0.8;
    float diff = max(dot(N, L), 0.0) * diffuseStrength;
    vec3 diffuse = diff * lightColor;

    float specStrength = 1.0;
    float shininess = 32.0;
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), shininess) * specStrength;
    vec3 specular = spec * lightColor;

    fragColor = vec4((ambient + diffuse) * objectColor + specular, 1.0);
}