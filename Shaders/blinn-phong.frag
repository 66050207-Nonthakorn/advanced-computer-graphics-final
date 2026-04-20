#version 330 core

struct Material {
    sampler2D diffuse;
    float specular;
    float shininess;
};

struct Light {
    vec3 position;
    float ambient;
    float diffuse;
    float specular;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 uv;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main() {
    vec3 N = normalize(normal);
    vec3 L = normalize(light.position - fragPosition);
    vec3 V = normalize(viewPosition - fragPosition);
    vec3 H = normalize(L + V);
    vec4 texColor = texture(material.diffuse, uv);

    vec3 ambient = texColor.rgb * light.ambient;

    vec3 diff = max(dot(N, L), 0.0) * texColor.rgb;
    vec3 diffuse = diff * light.diffuse;

    float spec = pow(max(dot(N, H), 0.0), material.shininess) * material.specular;
    vec3 specular = spec * light.specular * vec3(1.0f);

    fragColor = vec4((ambient + diffuse + specular), texColor.a);
}