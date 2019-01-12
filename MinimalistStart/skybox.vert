#version 400
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

out vec3 var_normal;
out vec3 var_position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	gl_Position = projection * vec4(mat3(view) * inPosition , 1.0); 
	var_normal = mat3(view) * inNormal;
	var_position = mat3(view) * inPosition * 10000;
}