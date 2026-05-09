#version 330 core

struct Material {
    sampler2D albedo;
    sampler2D ao;
    sampler2D metallic;
    sampler2D normal;
    sampler2D roughness;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

const float PI = 3.14159265359;
const int MAX_LIGHT = 10;

in vec3 normal;
in vec3 worldPosition;
in vec2 uv;

out vec4 fragColor;

uniform vec3 viewPosition;
uniform float normalStrength;
uniform Material material;
uniform bool useAlbedoMap;
uniform bool useAoMap;
uniform bool useMetallicMap;
uniform bool useNormalMap;
uniform bool useRoughnessMap;
uniform vec3  albedoColor;
uniform float metallicValue;
uniform float roughnessValue;

uniform int pointLightCount;
uniform PointLight pointLights[MAX_LIGHT];
uniform DirectionalLight directionalLight;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

// Debug: 0=off, 1=shadow only, 2=ambient only, 3=diffuse only, 4=specular only
uniform int debugMode;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;
uniform float       iblPrefilterMaxLod;
uniform float       iblIntensity;
uniform bool        useIBL;

vec3 getNormalFromMap() {
    if (!useNormalMap) {
        return normalize(normal);
    }

    vec3 tangentNormal = texture(material.normal, uv).xyz * 2.0 - 1.0;
    tangentNormal.xy *= normalStrength;

    vec3 Q1  = dFdx(worldPosition);
    vec3 Q2  = dFdy(worldPosition);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N = normalize(normal);

    vec3 T = Q1 * st2.y - Q2 * st1.y;
    T = normalize(T);
    T = normalize(T - dot(T, N) * N); // Gram-Schmidt orthogonalization
    
    float det = (st1.x * st2.y - st2.x * st1.y);
    vec3 B = normalize(cross(T, N)) * (det < 0.0 ? -1.0 : 1.0);

    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float ggx1 = geometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx2 = geometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Fresnel with roughness bias — used for IBL ambient term
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float shadowCalculation(vec4 fragPosLightSpace, vec3 N, vec3 L) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    vec3  albedo     = useAlbedoMap ? texture(material.albedo, uv).rgb : albedoColor;
    float metallic   = useMetallicMap ? texture(material.metallic, uv).r : metallicValue;
    float roughness  = useRoughnessMap ? texture(material.roughness, uv).r : roughnessValue;
    float ao         = useAoMap ? texture(material.ao, uv).r : 0.3;

    roughness = clamp(roughness, 0.04, 0.9);

    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPosition - worldPosition);

    vec3 F0 = vec3(0.02);
    F0 = mix(F0, albedo, metallic);

    // Separate diffuse and specular for debug visibility
    vec3 Lo_diff = vec3(0.0);
    vec3 Lo_spec = vec3(0.0);

    // Point lights
    for(int i = 0; i < pointLightCount; i++) {
        vec3 L = normalize(pointLights[i].position - worldPosition);
        vec3 H = normalize(V + L);

        float dist = length(pointLights[i].position - worldPosition);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

        float D = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 numerator = D * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 spec = numerator / denom;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        Lo_diff += kD * albedo / PI * radiance * NdotL;
        Lo_spec += spec * radiance * NdotL;
    }

    // Directional light contribution
    float dirShadow = 0.0;
    {
        vec3 L = normalize(-directionalLight.direction);
        vec3 H = normalize(V + L);
        vec3 radiance = directionalLight.color * directionalLight.intensity;

        float D = distributionGGX(N, H, roughness);
        float G = geometrySmith(N, V, L, roughness);
        vec3  F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 numerator = D * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 spec = numerator / denom;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);

        vec4 fragPosLightSpace = lightSpaceMatrix * vec4(worldPosition, 1.0);
        dirShadow = shadowCalculation(fragPosLightSpace, N, L);
        float lit = 1.0 - dirShadow;

        Lo_diff += kD * albedo / PI * radiance * NdotL * lit;
        Lo_spec += spec * radiance * NdotL * lit;
    }

    // Ambient / IBL — separated into diffuse and specular for debug
    vec3 amb_diff = vec3(0.03) * albedo * ao;
    vec3 amb_spec = vec3(0.0);
    if (useIBL) {
        float NdotV = max(dot(N, V), 0.0);
        vec3 F_ibl = fresnelSchlickRoughness(NdotV, F0, roughness);
        vec3 kD_ibl = (1.0 - F_ibl) * (1.0 - metallic);

        vec3 irradiance = texture(irradianceMap, N).rgb;
        amb_diff = kD_ibl * irradiance * albedo * ao * iblIntensity;

        // Specular — clamp prefiltered color to guard against HDR fireflies from bright stars/sun
        vec3 R = reflect(-V, N);
        float mip = roughness * iblPrefilterMaxLod;
        vec3 prefilteredColor = min(textureLod(prefilterMap, R, mip).rgb, vec3(50.0));
        vec2 brdf = texture(brdfLUT, vec2(NdotV, roughness)).rg;
        amb_spec = prefilteredColor * (F_ibl * brdf.x + brdf.y) * ao * iblIntensity;
    }

    // Debug modes
    if (debugMode == 1) {
        // Shadow only — grayscale; 1.0 = fully in shadow
        fragColor = vec4(vec3(dirShadow), 1.0);
        return;
    }
    if (debugMode == 2) {
        // Ambient only (IBL diffuse + IBL specular)
        vec3 c = amb_diff + amb_spec;
        c = c / (c + vec3(1.0));
        c = pow(c, vec3(1.0 / 2.2));
        fragColor = vec4(c, 1.0);
        return;
    }
    if (debugMode == 3) {
        // Diffuse only (direct + IBL diffuse)
        vec3 c = Lo_diff + amb_diff;
        c = c / (c + vec3(1.0));
        c = pow(c, vec3(1.0 / 2.2));
        fragColor = vec4(c, 1.0);
        return;
    }
    if (debugMode == 4) {
        // Specular only (direct + IBL specular)
        vec3 c = Lo_spec + amb_spec;
        c = c / (c + vec3(1.0));
        c = pow(c, vec3(1.0 / 2.2));
        fragColor = vec4(c, 1.0);
        return;
    }

    vec3 ambient = amb_diff + amb_spec;
    vec3 color = ambient + Lo_diff + Lo_spec;
    color = color / (color + vec3(1.0)); // HDR Tonemapping (Reinhard)
    color = pow(color, vec3(1.0 / 2.2)); // Gamma correction

    fragColor = vec4(color, 1.0);
}