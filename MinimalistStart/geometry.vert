#version 130
#extension GL_ARB_explicit_attrib_location : enable

#ifndef TANGETNS_AND_TEXCOORDS
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
#else
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTexcoords;
layout (location = 3) in vec3 inTangents;
#endif

#if defined(INSTANCING) && defined(TANGETNS_AND_TEXCOORDS)
layout (location = 4) in vec4 inModelA;
layout (location = 5) in vec4 inModelB;
layout (location = 6) in vec4 inModelC;
layout (location = 7) in vec4 inModelD;
#endif

out vec3 var_material;
out vec3 var_normal;
out vec3 var_position;

uniform mat4 projection;

#ifndef INSTANCING
uniform mat4 model;
#endif

uniform mat4 view;
uniform vec3 mrao;

void main()
{
	var_material = mrao.yxz;
#ifdef INSTANCING
	mat4 model = mat4(inModelA, inModelB, inModelC, inModelD);
#endif

#ifdef SKYBOX
	gl_Position = projection * vec4(mat3(view) * inPosition , 1.0); 
	var_normal = mat3(view) * inNormal;
	var_position = mat3(view) * inPosition * 10000;
#else
	gl_Position = projection * view * model * vec4(inPosition, 1.0); 
	var_normal = mat3(model) * inNormal;
	var_position = (model * vec4(inPosition, 1.0)).xyz;
#endif
}