#version 130
out vec4 FragColor;

in vec2 frag_texcoord;

uniform sampler2D color;
uniform sampler2D position;
uniform sampler2D normal;

void main()
{
    vec3 positionV =  texture(position, frag_texcoord).xyz;
	vec3 normalV =    texture(normal, frag_texcoord).xyz;
	vec3 colorV =     pow(texture(color, frag_texcoord).xyz, vec3(2.2));

	vec3 L = normalize(vec3(-1.0));
	float NdotL = dot(-L, normalV);
	vec3 L0 = colorV * clamp(NdotL, 0.0, 1.0) + colorV * 0.1;
    FragColor = vec4(pow(L0, vec3(1.0/2.2)), 1.0);
}