#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 positon;

out vec2 frag_texcoord;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	frag_texcoord = positon * 0.5 + 0.5;
}
