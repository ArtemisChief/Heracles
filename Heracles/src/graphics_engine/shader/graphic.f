#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	FragColor = vec4(0.57f, 0.78f, 0.39f, 1.0f);
}