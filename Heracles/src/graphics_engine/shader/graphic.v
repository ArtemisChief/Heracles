#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 translation;
uniform mat2 rotation;
uniform vec2 view;
uniform mat2 projection;
uniform vec2 anchor;

void main()
{
	gl_Position = mat4(vec4(projection[0], 0, 0),
	                   vec4(projection[1], 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(0, 0, 0, 1))
				* mat4(vec4(rotation[0], 0, 0),
	                   vec4(rotation[1], 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(translation - view + anchor, 0, 1))
	            * vec4(aPos - anchor, 0, 1);
}