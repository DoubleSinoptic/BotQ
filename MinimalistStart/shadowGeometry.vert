#version 400
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoords;
layout (location = 3) in vec3 inTangents;

#if defined(INSTANCING)
layout (location = 4) in vec4 inModelA;
layout (location = 5) in vec4 inModelB;
layout (location = 6) in vec4 inModelC;
layout (location = 7) in vec4 inModelD;
#endif

uniform mat4 projection;

#ifndef INSTANCING
uniform mat4 model;
#endif

uniform mat4 view;

void main()
{
#ifdef INSTANCING
	mat4 model = mat4(inModelA, inModelB, inModelC, inModelD);
#endif
	gl_Position = projection * view * model * vec4(inPosition, 1.0); 

}