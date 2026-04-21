#version 330 core

struct Material {
    sampler2D diffuse;
    float specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

in vec3 normal;
in vec3 worldPosition;
in vec2 uv;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(light.position - worldPosition);
    vec3 V = normalize(viewPosition - worldPosition);
    vec3 H = normalize(L + V);

    vec4 texColor = texture(material.diffuse, uv);

    vec3 ambient = 0.1f * texColor.rgb * light.color;

    vec3 diff = max(dot(N, L), 0.0) * texColor.rgb;
    vec3 diffuse = 0.6f * diff * light.color;

    float spec = pow(max(dot(N, H), 0.0), material.shininess) * material.specular;
    vec3 specular = 1.0f * spec * light.color * vec3(1.0f);

    fragColor = vec4((ambient + diffuse + specular), texColor.a);
}