#version 330 core

layout (location = 0) in vec2 in_vertex;
layout (location = 1) in vec2 in_tex_coord;

out vec2 tex_coord;

void main() {
	gl_Position = vec4(in_vertex, 0.0, 1.0);
	tex_coord = in_tex_coord;
}
