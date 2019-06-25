#version 400
#extension GL_ARB_explicit_attrib_location : enable

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D color;
uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D ssao;
uniform vec3 lookPosition;
uniform sampler2D rmo;

uniform mat4 shadowVP;
uniform sampler2D shadowDepth;



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


const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float cTonemapExposureBias = 6.0f;
float cTonemapMaxWhite = 11.0f;



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
	vec2 texelSize = 1.0 / textureSize(shadowDepth, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowDepth, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.3;
	return shadow;
}

vec3 aces(vec3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}


vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
	mat3 ACESInputMat = mat3(vec3(
	0.59719, 0.35458, 0.04823), vec3(
	0.07600, 0.90834, 0.01566), vec3(
	0.02840, 0.13383, 0.83777));
	mat3 ACESOutputMat = mat3(vec3(
	1.60475, -0.53108, -0.07367), vec3(
	-0.10208,  1.10813, -0.00605), vec3(
	-0.00327, -0.07276,  1.07602));

    color = color *ACESInputMat  ;

    color = RRTAndODTFit(color);

    color =color *ACESOutputMat ;

    color = clamp(color, vec3(0.0), vec3(1.0));

    return color;
}
vec3 LinearTosRGB(vec3 color)
{
    vec3 x = color * 12.92;
    vec3 y = 1.055 * pow(clamp(color, vec3(0.0), vec3(1.0)), vec3(1.0 / 2.4)) - 0.055;

    vec3 clr = color;
    clr.r = color.r < 0.0031308 ? x.r : y.r;
    clr.g = color.g < 0.0031308 ? x.g : y.g;
    clr.b = color.b < 0.0031308 ? x.b : y.b;

    return clr;
}
void main()
{
	float pcfDepth = texture(shadowDepth, texCoords).r; 
    vec3 positionV =  texture(position, texCoords).xyz;
	vec3 normalV =    texture(normal, texCoords).xyz;
	vec3 occolusion = texture(ssao, texCoords).xyz;
	vec3 colorV =     pow(texture(color, texCoords).xyz, vec3(2.2));
	vec3 rmo3 =        texture(rmo, texCoords).xyz;

	if(normalV == vec3(0.0))
	{
		vec3 final = 
	    Uncharted2Tonemap(max(colorV * cTonemapExposureBias, 0.0)) / 
        Uncharted2Tonemap(vec3(cTonemapMaxWhite, cTonemapMaxWhite, cTonemapMaxWhite));

		FragColor = vec4(pow(final , vec3(1.0/2.2)), 1.0);
		return;
	}

	vec3 Lo = vec3(0.0);
	
	vec3 V = normalize(lookPosition - positionV);
	vec3 N = normalV;
	
	float mullCoff = 1.0f;
	float dt = dot(normalize(-vec3(1, -1, 1)), N);
	if(dt >= 0)
	{
		float val = calculateShadows(shadowVP * vec4(positionV, 1));
		mullCoff = clamp(1.0 - val, 0.0, 1.0);
	}

	ULight light;
	light.color = pow(vec3(1.0, 0.95, 0.83), vec3(2.2)) * 29;
	light.dir = normalize(-vec3(-1.0, 1.0, -1.0));
	USurfaceSettings surf;
	surf.rougness = rmo3.x;
	surf.metalic = rmo3.y;
	surf.normal = N;
	surf.albedo = colorV;
	Lo += ue_brdf_base(light, surf, -V)  * rmo3.z * mullCoff;  
	

	vec3 ambient = vec3(0.33) * colorV * occolusion;
	vec3 rezult = ambient + Lo * occolusion;

#ifdef USE_ACES_UNCHARTED
	vec3 final = 
	    Uncharted2Tonemap(max(rezult * cTonemapExposureBias, 0.0)) / 
        Uncharted2Tonemap(vec3(cTonemapMaxWhite, cTonemapMaxWhite, cTonemapMaxWhite));
#else
	vec3 final = aces(rezult) ;
#endif
    FragColor = vec4(LinearTosRGB(final), 1.0);
	//FragColor = vec4(vec3(1.0 - pcfDepth), 1.0);
	
}