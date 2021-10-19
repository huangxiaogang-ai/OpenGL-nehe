#version 330

in vec3 VertColor;
in vec2 TextCoord;

uniform sampler2D tex;

out vec4 color;


void main()
{
	color = texture(tex, TextCoord) * vec4(VertColor, 1.0f);
}