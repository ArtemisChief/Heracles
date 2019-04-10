#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 translation;
uniform mat2 rotation;
uniform vec2 view;
uniform mat2 projection;

out vec2 TexCoord;

void main()
{
	gl_Position = mat4(vec4(projection[0], 0, 0),
	                   vec4(projection[1], 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(0, 0, 0, 1))
				* mat4(vec4(rotation[0], 0, 0),
	                   vec4(rotation[1], 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(translation - view, 0, 1))
	            * vec4(aPos, 0, 1);

	TexCoord = aTexCoord;
}