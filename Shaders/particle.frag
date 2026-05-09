#version 330 core

in float brightness;
in vec2  uv;
in float alpha;

out vec4 fragColor;

void main() {
    vec2 centeredUv = uv * 2.0 - 1.0;
    float dist = length(centeredUv);
    float edgeFade = 1.0 - smoothstep(0.25, 1.0, dist);
    float a = alpha * edgeFade;

    if (a <= 0.001) {
        discard;
    }

    fragColor = vec4(vec3(brightness), a);
}
