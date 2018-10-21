#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 positon;


out vec2 texel;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	texel = positon * 0.5 + 0.5;
}
&
#version 130
out vec4 FragColor;

in vec2 texel;

uniform sampler2D input;

void main()
{
	const float hfMul = 30.0;
	vec2 pixelExtents = vec2(1.0) / vec2(textureSize(input, 0));
	float glow = 4.0 * ((pixelExtents.x + pixelExtents.y) / 2.0);
	vec3 bloom = vec3(0);
	int count = 0;
	for(float x = texel.x - glow; x < texel.x + glow; x += pixelExtents.x)
	{
		for(float y = texel.y - glow; y < texel.y + glow; y += pixelExtents.y)
		{	
			if(length(vec2(x, y) - texel) <= glow)
			{
				bloom += (texture(input, vec2(x, y)).rgb - 0.4) * hfMul;
				count++;
			}			
		}
	}
    FragColor = vec4(clamp(bloom / (count * hfMul), 0.0, 1.0), 1.0);
}