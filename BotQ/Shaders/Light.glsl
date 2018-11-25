#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) in vec2 positon;


out vec2 fragTexcoord;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	fragTexcoord = positon * 0.5 + 0.5;
}


&

#version 130
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
layout ( location = 0 ) out vec4 RezFragColor;
layout ( location = 1 ) out vec4 RezBrightColor;
layout ( location = 2 ) out vec4 RezGodRay;

in vec2 fragTexcoord;

layout ( location = 23 ) uniform sampler2D FragPosition;
uniform sampler2D FragNormal;
uniform sampler2D FragColor;
uniform sampler2D FragSettings;
uniform sampler2D FragDepth;
uniform sampler2D FragSSAO;
uniform sampler2D FragPBR;
uniform bool debugDrawShadowProjection;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform vec3	  lookPosition;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform bool ssaoEnabled;

vec3 fragPos;
vec3 fragNorm;
vec4 fragColor;
vec4 fragSett;
vec4 fragDepth;
vec4 fragPBR;
float ssaoAtt;
float shadowLume = 0.0;
float assymp = 0.0;


vec3 LinearToGammaSpace (vec3 color)
{
    color = max(color, vec3(0, 0, 0));
    return max(1.055 * pow(color, vec3(0.416666667)) - 0.055, 0);
}

vec3 GammaToLinearSpace (vec3 color)
{
    return color * (color * (color * 0.305306011 + 0.682171111) + 0.012522878);
}



// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
#ifdef USE_IBL
vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
    float mipCount = 9.0; // resolution of 512x512
    float lod = (pbrInputs.perceptualRoughness * mipCount);
    // retrieve a scale and bias to F0. See [1], Figure 3
    vec3 brdf = SRGBtoLINEAR(texture2D(u_brdfLUT, vec2(pbrInputs.NdotV, 1.0 - pbrInputs.perceptualRoughness))).rgb;
    vec3 diffuseLight = SRGBtoLINEAR(textureCube(u_DiffuseEnvSampler, n)).rgb;

#ifdef USE_TEX_LOD
    vec3 specularLight = SRGBtoLINEAR(textureCubeLodEXT(u_SpecularEnvSampler, reflection, lod)).rgb;
#else
    vec3 specularLight = SRGBtoLINEAR(textureCube(u_SpecularEnvSampler, reflection)).rgb;
#endif

    vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
    vec3 specular = specularLight * (pbrInputs.specularColor * brdf.x + brdf.y);

    // For presentation, this allows us to disable IBL terms
    diffuse *= u_ScaleIBLAmbient.x;
    specular *= u_ScaleIBLAmbient.y;

    return diffuse + specular;
}
#endif

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

struct ULight
{
   vec3 color;
   vec3 dir;
};

struct USurfaceSettings
{
  vec3 albedo;
  float rougness;
  float metalic;
  vec3 normal;
};

vec3 ue_brdf_base(ULight light, USurfaceSettings surf, vec3 viewDir)
{
    // Metallic and Roughness material properties are packed together
    // In glTF, these factors can be specified by fixed scalar values
    // or from a metallic-roughness map
    float perceptualRoughness = surf.rougness;
    float metallic = surf.metalic;

    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    // The albedo may be defined from a base texture or a flat color

    vec4 baseColor = vec4(surf.albedo, 1.0);


    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 n = surf.normal;                             // normal at surface point
    vec3 v = normalize(-viewDir);        // Vector from surface point to camera
    vec3 l = normalize(-light.dir);             // Vector from surface point to light
    vec3 h = normalize(l+v);                          // Half vector between both l and v
    vec3 reflection = -normalize(reflect(v, n));

    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);

    PBRInfo pbrInputs = PBRInfo(
        NdotL,
        NdotV,
        NdotH,
        LdotH,
        VdotH,
        perceptualRoughness,
        metallic,
        specularEnvironmentR0,
        specularEnvironmentR90,
        alphaRoughness,
        diffuseColor,
        specularColor
    );

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = specularReflection(pbrInputs);
    float G = geometricOcclusion(pbrInputs);
    float D = microfacetDistribution(pbrInputs);

    // Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    vec3 color = NdotL * light.color * (diffuseContrib + specContrib);

    // Calculate lighting contribution from image based lighting source (IBL)
#ifdef USE_IBL
    color += getIBLContribution(pbrInputs, n, reflection);
#endif

    // Apply optional PBR terms for additional (optional) shading

    return color;
}

struct LightSource
{
        vec3 position;
        vec3 color;
		bool inFragSpace;
};

uniform LightSource lights[64];
uniform int lightsCount;

float calculateShadows(vec4 fragPosLightSpace)
{
	 vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; 


	#ifdef DEBUG_DRAW_PROJ
	if(debugDrawShadowProjection)
	{	
		if(
		((projCoords.x > 1.0) && (projCoords.x < 1.0015) && ((projCoords.y <= 1.0) && (projCoords.z >= 0.00) && (projCoords.z <= 1.0) && (projCoords.y >= 0.00))) ||
		((projCoords.x < 0.0) && (projCoords.x > -0.0015) && ((projCoords.y <= 1.0)  && (projCoords.z >= 0.00) && (projCoords.z <= 1.0) && (projCoords.y >= 0.00))) ||
		((projCoords.y > 1.0) && (projCoords.y < 1.0015) && ((projCoords.x <= 1.0)  && (projCoords.z >= 0.00) && (projCoords.z <= 1.0) && (projCoords.x >= 0.00))) ||
		((projCoords.y < 0.0) && (projCoords.y > -0.0015) && ((projCoords.x <= 1.0)  && (projCoords.z >= 0.00) && (projCoords.z <= 1.0) && (projCoords.x >= 0.00))) 
		)
		{
			shadowLume = 1.0;
			return 0.0;
		}
	}
	#endif
	if(projCoords.x > 1.0 || projCoords.x < 0.0 || projCoords.y > 1.0 || projCoords.y < 0.0)
		return 0.0;

	float currentDepth = projCoords.z;
	if(currentDepth > 1.0)
		return 0.0;
	float bias = 0.000;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.3;
	return shadow;
}

const vec3 ToD[7] = vec3[7](  vec3(0.58597,0.15,0.02),
								vec3(0.58597,0.35,0.09),
								vec3(0.58597,0.5,0.26),
								vec3(0.58597,0.5,0.35),
								vec3(0.58597,0.5,0.36),
								vec3(0.58597,0.5,0.37),
								vec3(0.58597,0.5,0.38));


const float PI = 3.14159265359;

// Нотация NVIDIA
// Дистрибутивность отражательной способности
//TODO 
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); 
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

/*
vec3 lg_brdf_base()
{
	vec3 L = normalize(lightPosition - fragPos);
	vec3 H = normalize(V + L);
	float distance = length(lightPosition - fragPos);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = pow(lights[i].color, vec3(2.2)) * 31 * attenuation;
		
	float NDF = DistributionGGX(N, H, roughness);   
	float G   = GeometrySmith(N, V, L, roughness);      
	vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
	vec3 nominator    = NDF * G * F; 
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = nominator / max(denominator, 0.001); 
             
	vec3 kS = F; 
	vec3 kD = vec3(1.0) - kS;
   
	kD *= 1.0 - metallic;	  
   
	float NdotL = max(dot(N, L), 0.0);        
	Lo += (kD * albedo / PI + specular) * radiance * NdotL; 	 

}
*/


void main()
{
	fragPos = texture(FragPosition, fragTexcoord).xyz;
    fragNorm = texture(FragNormal, fragTexcoord).xyz;
	fragColor = texture(FragColor, fragTexcoord);
	fragSett = texture(FragSettings, fragTexcoord);
	fragDepth = texture(FragDepth, fragTexcoord);
	fragPBR = texture(FragPBR, fragTexcoord);
	if(ssaoEnabled)
	{
		float ssaoAttantion = texture(FragSSAO, fragTexcoord).r;
		ssaoAttantion = pow(ssaoAttantion, 2.0);
		fragColor =  vec4(vec3(ssaoAttantion) * fragColor.xyz, fragColor.w);
	}

	//fragColor *= ssaoAttantion;

	if(fragSett.z > 0)
	{
		RezBrightColor = vec4(fragColor.xyz, 1.0);
		RezFragColor = vec4(fragColor.xyz , 1.0);
		RezGodRay = vec4(fragColor.xyz , 1.0);
		return;
	}
	RezGodRay = vec4(vec3(0.0), 1.0);
	vec3 color;


	vec3 albedo = GammaToLinearSpace(fragColor.xyz);



	float roughness = fragPBR.y;
	float ao = fragPBR.z;
	float metallic = fragPBR.x;

	vec3 N = normalize(fragNorm);
    vec3 V = normalize(lookPosition - fragPos);
	vec3 R = reflect(-V, N); 
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	float mullCoff = 1.0f;
	float dt = dot(normalize(-vec3(1, -1, 1)), N);
	if(dt >= 0)
	{
		float val = calculateShadows(fragDepth);
		mullCoff = clamp(1.0 - val, 0.0, 1.0);
	}

	for(int i = 0; i <lightsCount; i++)
	{		
		vec3 lightPosition;
		if(lights[i].inFragSpace)
			lightPosition=  fragPos + lights[i].position;
		else
			lightPosition= lights[i].position;
		 
		ULight light;
		light.color = GammaToLinearSpace(lights[i].color ) * 16;
		light.dir = -lights[i].position;
		USurfaceSettings surf;
		surf.rougness = roughness;
		surf.metalic = metallic;
		surf.normal = N;
		surf.albedo = albedo;
		Lo += ue_brdf_base(light, surf, -V) * ao;  

	}
	

#define PERFORM_CUBEMAP
#ifdef PERFORM_CUBEMAP
		vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;	  
    
		vec3 irradiance = texture(irradianceMap, N).rgb;
		vec3 diffuse      = irradiance * albedo;

		const float MAX_REFLECTION_LOD = 10.0;
		vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
		vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);


		vec3 ambient = (kD * diffuse + specular) * ao;
	
#else
		vec3 ambient = vec3(0.33) * albedo * ao;
	
#endif
	
	
    color = ambient + Lo * mullCoff;
	
	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
#define BLUR_MODEL_A
#ifdef BLUR_MODEL_A
	if(brightness > 2.96)
		RezBrightColor = vec4(vec3(0.0) , fragColor.a);
	else
		RezBrightColor = vec4(color / 2, fragColor.a);
	 RezFragColor = vec4(color, fragColor.a);
#else
	RezBrightColor = vec4(color , fragColor.a);
	 RezFragColor = vec4(color, fragColor.a);
#endif
}