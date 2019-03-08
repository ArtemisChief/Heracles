#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 translation;
uniform mat2 rotation;
uniform mat2 view;
uniform mat2 projection;

void main()
{
	gl_Position = mat4(vec4(projection[0], 0, 0),
	                   vec4(projection[1], 0, 0),
					   vec4(0, 0, 1, 0),
					   vec4(0, 0, 0, 1))
			      * mat4(vec4(view[0], 0, 0),
	                   vec4(view[1], 0, 0),
					   vec4(0, 0, 1, 0),
					   vec4(0, 0, 0, 1))
				  * mat4(vec4(rotation[0], 0, 0),
	                   vec4(rotation[1], 0, 0),
					   vec4(0, 0, 1, 0),
					   vec4(translation, 0, 1))
	              * vec4(aPos, 0, 1);
}