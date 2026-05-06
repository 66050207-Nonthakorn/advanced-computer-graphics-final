#version 330 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D diffuse;
uniform bool useTexture;
uniform vec4 color;

void main() {
    if (useTexture) {
        vec4 tex = texture(diffuse, uv);
        if (tex.a < 0.01) discard;
        fragColor = tex * color;
    }
    else {
        fragColor = color;
    }
}
