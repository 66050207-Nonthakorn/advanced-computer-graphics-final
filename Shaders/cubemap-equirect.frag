#version 330 core

in vec3 uv;
out vec4 fragColor;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v) {
    vec2 mapped = vec2(atan(v.z, v.x), asin(v.y));
    mapped *= invAtan;
    mapped += 0.5;
    return mapped;
}

void main() {
    vec2 texCoord = sampleSphericalMap(normalize(uv));
    vec3 color = texture(equirectangularMap, texCoord).rgb;
    fragColor = vec4(color, 1.0);
}