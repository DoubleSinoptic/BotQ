#version 400
#extension GL_ARB_explicit_attrib_location : enable
layout ( location = 0 ) out vec3 OutFragColor;
layout ( location = 1 ) out vec3 OutPosition;
layout ( location = 2 ) out vec3 OutNormal;
layout ( location = 3 ) out vec3 OutMaterial;

in vec2 var_texcoord;
in vec3 var_normal;
in vec3 var_position;
in vec3 var_material;
in mat3 var_tbn;

uniform bool is_roughness;
uniform bool is_metalic;
uniform bool is_ao;
uniform bool is_albedo;
uniform bool is_normal;

uniform vec3		static_albedo;
uniform sampler2D	map_roughness;
uniform sampler2D	map_metalic;
uniform sampler2D	map_ao;
uniform sampler2D	map_albedo;
uniform sampler2D	map_normal;

void main()
{
	vec3 material = var_material;
	
	if(is_roughness)
		material.x = texture(map_roughness, var_texcoord).r;	
	if(is_metalic)
		material.y = texture(map_metalic, var_texcoord).r;	
	if(is_ao)
		material.z = texture(map_ao, var_texcoord).r;	
	OutMaterial = material;

	if(is_normal)
	{
		vec3 N = texture(map_normal, var_texcoord).rgb;
		N = normalize(N * 2.0 - 1.0);   
		OutNormal = normalize(var_tbn * N); 
	}
	else
		OutNormal = normalize(var_normal);

	if(is_albedo)
		OutFragColor = texture(map_albedo, var_texcoord).rgb;	
	else	
		OutFragColor = static_albedo;	
	OutPosition = var_position; 	
}