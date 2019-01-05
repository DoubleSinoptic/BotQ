#version 130 
#extension GL_ARB_explicit_attrib_location : enable

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D texNoise;

uniform vec2 renderSize;
uniform vec3 kernel[64];
uniform int sampleCount;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    float radius = 0.5;
    float bias = 0.025;
    vec2 noiseScale = renderSize.xy * vec2(1.0 / 4.0);
    
    vec4 normalRoughness  = texture(normal, texCoords);
	vec4 positionMetallic = texture(position, texCoords);
    vec3 randomVec        = texture(texNoise, texCoords * noiseScale).xyz;
    
	if(normalRoughness.xyz == vec3(0.0))
	{	
		return;
	}

    vec3 fragPos = (view * vec4(positionMetallic.xyz, 1.0)).xyz;
    vec3 normal  = mat3(view) * normalRoughness.xyz;
    
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = normalize(cross(normal, tangent));
    mat3 TBN       = mat3(tangent, bitangent, normal);  
    
    float occlusion = 0.0;
    for(int i = 0; i < sampleCount; ++i)
    {    
		vec3 sample = TBN * kernel[i]; 
        sample = fragPos + sample * radius; 
            
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; 
        offset.xyz /= offset.w; 
        offset.xyz = offset.xyz * 0.5 + 0.5; 
           
        float sampleDepth = (view * vec4(texture(position, offset.xy).xyz, 1.0)).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / float(sampleCount));
    occlusion = pow(occlusion, 3.0);

    FragColor = vec4(vec3(occlusion), 1.0);
}