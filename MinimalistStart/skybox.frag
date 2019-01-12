#version 400
#extension GL_ARB_explicit_attrib_location : enable

layout ( location = 0 ) out vec3 OutFragColor;
layout ( location = 1 ) out vec3 OutPosition;
layout ( location = 2 ) out vec3 OutNormal;
layout ( location = 3 ) out vec3 OutMaterial;

in vec3 var_normal;
in vec3 var_position;

void main()
{

	OutFragColor = vec3(1.0);	
	OutPosition = var_position; 	
	OutNormal = vec3(0.0, 0.0, 0.0); 
	OutMaterial = vec3(0.0);

}