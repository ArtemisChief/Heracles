#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform vec2 view;
uniform mat2 projection;

void main()
{
    gl_Position = mat4(vec4(projection[0], 0, 0),
	                   vec4(projection[1], 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(0, 0, 0, 1))
			    * mat4(vec4(1, 0, 0, 0),
	                   vec4(0, 1, 0, 0),
					   vec4(0, 0, 0, 0),
					   vec4(-view, 0, 1))
				* vec4(vertex.xy, 0, 1);
    TexCoords = vertex.zw;
}