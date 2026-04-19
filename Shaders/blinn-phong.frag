#version 330 core

struct Material {
    float ambient = 0.1;
    float diffuse = 0.8;
    float specular = 1.0;
    float shininess = 128.0;
}

in vec3 normal;
in vec3 fragPosition;
in vec2 uv;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform sampler2D objectTexture;
uniform Material material;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 V = normalize(viewPosition - fragPosition);
    vec3 H = normalize(L + V);

    vec3 ambient = material.ambient * lightColor;

    float diff = max(dot(N, L), 0.0) * material.diffuse;
    vec3 diffuse = diff * lightColor;

    float spec = pow(max(dot(N, H), 0.0), material.shininess) * material.specular;
    vec3 specular = spec * lightColor;

    vec4 texColor = texture(objectTexture, uv);
    fragColor = vec4((ambient + diffuse) * texColor.rgb + specular, texColor.a);
}