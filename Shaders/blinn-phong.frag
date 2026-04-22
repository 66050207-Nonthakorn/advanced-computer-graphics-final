#version 330 core

struct Material {
    sampler2D diffuse;
    float specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

const int MAX_LIGHT = 10;

in vec3 normal;
in vec3 worldPosition;
in vec2 uv;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform Material material;

uniform int pointLightCount;
uniform Light pointLights[10];
uniform DirectionalLight directionalLight;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

float shadowCalculation(vec4 fragPosLS, vec3 N, vec3 L) {
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPosition - worldPosition);

    vec4 texColor = texture(material.diffuse, uv);
    vec3 ambient = 0.1 * texColor.rgb;

    vec3 Lo = vec3(0.0);

    // Point lights
    for (int i = 0; i < pointLightCount; i++) {
        vec3 L = normalize(pointLights[i].position - worldPosition);
        vec3 H = normalize(L + V);
        float dist = length(pointLights[i].position - worldPosition);
        float attenuation = 1.0 / (dist * dist);
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse  = NdotL * texColor.rgb * pointLights[i].color * pointLights[i].intensity * attenuation;
        float spec    = pow(max(dot(N, H), 0.0), material.shininess) * material.specular;
        vec3 specular = spec * pointLights[i].color * pointLights[i].intensity * attenuation;
        Lo += diffuse + specular;
    }

    // Directional light + shadow
    vec3 L_dir = normalize(-directionalLight.direction);
    vec3 H_dir = normalize(L_dir + V);
    vec4 fragPosLS = lightSpaceMatrix * vec4(worldPosition, 1.0);
    float shadow = shadowCalculation(fragPosLS, N, L_dir);
    vec3 diffuse_dir  = (1.0 - shadow) * max(dot(N, L_dir), 0.0) * texColor.rgb * directionalLight.color * directionalLight.intensity;
    float spec_dir    = pow(max(dot(N, H_dir), 0.0), material.shininess) * material.specular;
    vec3 specular_dir = (1.0 - shadow) * spec_dir * directionalLight.color * directionalLight.intensity;

    vec3 color = ambient + Lo + diffuse_dir + specular_dir;
    fragColor = vec4(color, texColor.a);
}