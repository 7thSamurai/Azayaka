#version 330 core

in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D texture_sampler;

void main() {
	frag_color = texture(texture_sampler, tex_coord);
}
