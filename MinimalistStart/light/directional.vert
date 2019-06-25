#version 400
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 positon;

out vec2 texCoords;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	texCoords = positon * 0.5 + 0.5;
}
