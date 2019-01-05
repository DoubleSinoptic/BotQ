#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout ( location = 0 ) out vec3 OutFragColor;
layout ( location = 1 ) out vec3 OutPosition;
layout ( location = 2 ) out vec3 OutNormal;

in vec3 var_normal;
in vec3 var_position;
uniform mat4 projection;
uniform mat4 model;

void main()
{
#ifdef SKYBOX
	OutFragColor = vec3(1.0);	
	OutPosition = var_position; 	
	OutNormal = vec3(0.0, 0.0, 0.0); 	
#else
	OutFragColor = vec3(1.0);	
	OutPosition = var_position; 	
	OutNormal = normalize(var_normal); 	
#endif
}