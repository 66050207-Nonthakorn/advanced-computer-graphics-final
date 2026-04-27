#version 330 core

layout (location = 0) in vec3 aPosition;

out vec3 uv;

uniform mat4 view;
uniform mat4 projection;

void main() {
	uv = aPosition;

	vec4 position = projection * mat4(mat3(view)) * vec4(aPosition, 1.0);
	gl_Position = position.xyww;
}
