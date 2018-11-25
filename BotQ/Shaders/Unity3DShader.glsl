precision highp float;
uniform float time;
uniform vec2 resolution;
varying vec3 fPosition;
varying vec3 fNormal;

const float UNITY_PI = 3.14159265359;
const float UNITY_INV_PI = 1.0 / UNITY_PI; 
#define UNITY_COLORSPACE_GAMMA
#define UNITY_BRDF_GGX

float saturate(float v)
{
  return clamp(v, 0.0, 1.0);
}

#define float4 vec4
#define half4 vec4
#define float3 vec3
#define half float
#define half3 vec3

float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

half RoughnessToPerceptualRoughness(half roughness)
{
    return sqrt(roughness);
}


struct UnityLight
{
    half3 color;
    half3 dir;
    half  ndotl; // Deprecated: Ndotl is now calculated on the fly and is no longer stored. Do not used it.
};

struct UnityIndirect
{
    half3 diffuse;
    half3 specular;
};

bool anyMZ(vec3 a)
{
  if(a.x != 0.0 || a.y != 0.0 || a.z != 0.0)
    return true;
    return false;
}

float3 Unity_SafeNormalize(float3 inVec)
{
    float dp3 = max(0.001, dot(inVec, inVec));
    return inVec * (1.0 /sqrt(dp3));
}

float Pow5 (float x)
{
    return x*x * x*x * x;
}

half3 FresnelLerp (half3 F0, half3 F90, half cosA)
{
    half t = Pow5 (1.0 - cosA);   // ala Schlick interpoliation
    return mix (F0, F90, t);
}

half3 FresnelTerm (half3 F0, half cosA)
{
    half t = Pow5 (1.0 - cosA);   // ala Schlick interpoliation
    return F0 + (1.0-F0) * t;
}

float GGXTerm (float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = (NdotH * a2 - NdotH) * NdotH + 1.0; // 2 mad
    return UNITY_INV_PI * a2 / (d * d + 1e-7); // This function is not intended to be running on Mobile,
                                            // therefore epsilon is smaller than what can be represented by half
}

half SmithJointGGXVisibilityTerm (half NdotL, half NdotV, half roughness)
{
    half a = roughness;
    half lambdaV = NdotL * (NdotV * (1.0 - a) + a);
    half lambdaL = NdotV * (NdotL * (1.0 - a) + a);

    return 0.5 / (lambdaV + lambdaL + 1e-5);
}

half NDFBlinnPhongNormalizedTerm (half NdotH, half n)
{
    // norm = (n+2)/(2*pi)
    half normTerm = (n + 2.0) * (0.5/UNITY_PI);

    half specTerm = pow (NdotH, n);
    return specTerm * normTerm;
}

half PerceptualRoughnessToSpecPower (half perceptualRoughness)
{
    half m = PerceptualRoughnessToRoughness(perceptualRoughness);   // m is the true academic roughness.
    half sq = max(1e-4, m*m);
    half n = (2.0 / sq) - 2.0;                          // https://dl.dropboxusercontent.com/u/55891920/papers/mm_brdf.pdf
    n = max(n, 1e-4);                                  // prevent possible cases of pow(0,0), which could happen when roughness is 1.0 and NdotH is zero
    return n;
}

half SmithVisibilityTerm (half NdotL, half NdotV, half k)
{
    half gL = NdotL * (1.0-k) + k;
    half gV = NdotV * (1.0-k) + k;
    return 1.0 / (gL * gV + 1e-5); // This function is not intended to be running on Mobile,
                                    // therefore epsilon is smaller than can be represented by half
}

half SmithBeckmannVisibilityTerm (half NdotL, half NdotV, half roughness)
{
    half c = 0.797884560802865; // c = sqrt(2 / Pi)
    half k = roughness * c;
    return SmithVisibilityTerm (NdotL, NdotV, k) * 0.25; // * 0.25 is the 1/4 of the visibility term
}


half SmoothnessToRoughness(half smoothness)
{
    return (1.0 - smoothness) * (1.0 - smoothness);
}

float SmoothnessToPerceptualRoughness(float smoothness)
{
    return (1.0 - smoothness);
}

half DisneyDiffuse(half NdotV, half NdotL, half LdotH, half perceptualRoughness)
{
    half fd90 = 0.5 + 2.0 * LdotH * LdotH * perceptualRoughness;
    // Two schlick fresnel term
    half lightScatter   = (1.0 + (fd90 - 1.0) * Pow5(1.0 - NdotL));
    half viewScatter    = (1.0 + (fd90 - 1.0) * Pow5(1.0 - NdotV));

    return lightScatter * viewScatter;
}

half4 BRDF1_Unity_PBS (half3 diffColor, half3 specColor, half oneMinusReflectivity, half smoothness,
    float3 normal, float3 viewDir,
    UnityLight light, UnityIndirect gi)
{
    float perceptualRoughness = SmoothnessToPerceptualRoughness (smoothness);
    float3 halfDir = Unity_SafeNormalize (float3(light.dir) + viewDir);


#define UNITY_HANDLE_CORRECTLY_NEGATIVE_NDOTV 0

#if UNITY_HANDLE_CORRECTLY_NEGATIVE_NDOTV
    // The amount we shift the normal toward the view vector is defined by the dot product.
    half shiftAmount = dot(normal, viewDir);
    normal = shiftAmount < 0.0f ? normal + viewDir * (-shiftAmount + 1e-5f) : normal;
    // A re-normalization should be applied here but as the shift is small we don't do it to save ALU.
    //normal = normalize(normal);

    half nv = saturate(dot(normal, viewDir)); // TODO: this saturate should no be necessary here
#else
    half nv = abs(dot(normal, viewDir));    // This abs allow to limit artifact
#endif

    half nl = saturate(dot(normal, light.dir));
    float nh = saturate(dot(normal, halfDir));

    half lv = saturate(dot(light.dir, viewDir));
    half lh = saturate(dot(light.dir, halfDir));

    // Diffuse term
    half diffuseTerm = DisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;

    // Specular term
    // HACK: theoretically we should divide diffuseTerm by Pi and not multiply specularTerm!
    // BUT 1) that will make shader look significantly darker than Legacy ones
    // and 2) on engine side "Non-important" lights have to be divided by Pi too in cases when they are injected into ambient SH
    float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);

    // GGX with roughtness to 0 would mean no specular at all, using max(roughness, 0.002) here to match HDrenderloop roughtness remapping.
    roughness = max(roughness, 0.002);
    half V = SmithJointGGXVisibilityTerm (nl, nv, roughness);
    float D = GGXTerm (nh, roughness);
    half specularTerm = V*D * UNITY_PI; // Torrance-Sparrow model, Fresnel is applied later

    specularTerm = sqrt(max(1e-4, specularTerm));

    // specularTerm * nl can be NaN on Metal in some cases, use max() to make sure it's a sane value
    specularTerm = max(0.0, specularTerm * nl);


    // surfaceReduction = Int D(NdotH) * NdotH * Id(NdotL>0) dH = 1/(roughness^2+1)
    half surfaceReduction;

    surfaceReduction = 1.0-0.28*roughness*perceptualRoughness;      // 1-0.28*x^3 as approximation for (1

    // To provide true Lambert lighting, we need to be able to kill specular completely.
    specularTerm *= anyMZ(specColor) ? 1.0 : 0.0;

    half grazingTerm = saturate(smoothness + (1.0-oneMinusReflectivity));
    half3 color =   diffColor * (gi.diffuse + light.color * diffuseTerm)
                    + specularTerm * light.color * FresnelTerm (specColor, lh)
                    + surfaceReduction * gi.specular * FresnelLerp (specColor, vec3(grazingTerm), nv);

    return half4(color, 1);
}

struct SurfaceOutputStandard
{
    vec3 Albedo;      // base (diffuse or specular) color
    float3 Normal;      // tangent space normal, if written
    half3 Emission;
    half Metallic;      // 0=non-metal, 1=metal
    // Smoothness is the user facing name, it should be perceptual smoothness but user should not have to deal with it.
    // Everywhere in the code you meet smoothness it is perceptual smoothness
    half Smoothness;    // 0=rough, 1=smooth
    half Occlusion;     // occlusion (default 1)
    float Alpha;        // alpha for transparencies
};


half3 PreMultiplyAlpha (half3 diffColor, half alpha, half oneMinusReflectivity, out half outModifiedAlpha)
{
    #if defined(_ALPHAPREMULTIPLY_ON)
        // NOTE: shader relies on pre-multiply alpha-blend (_SrcBlend = One, _DstBlend = OneMinusSrcAlpha)

        // Transparency 'removes' from Diffuse component
        diffColor *= alpha;

        #if (SHADER_TARGET < 30)
            // SM2.0: instruction count limitation
            // Instead will sacrifice part of physically based transparency where amount Reflectivity is affecting Transparency
            // SM2.0: uses unmodified alpha
            outModifiedAlpha = alpha;
        #else
            // Reflectivity 'removes' from the rest of components, including Transparency
            // outAlpha = 1-(1-alpha)*(1-reflectivity) = 1-(oneMinusReflectivity - alpha*oneMinusReflectivity) =
            //          = 1-oneMinusReflectivity + alpha*oneMinusReflectivity
            outModifiedAlpha = 1-oneMinusReflectivity + alpha*oneMinusReflectivity;
        #endif
    #else
        outModifiedAlpha = alpha;
    #endif
    return diffColor;
}
#define unity_ColorSpaceDielectricSpec half4(0.220916301, 0.220916301, 0.220916301, 1.0 - 0.220916301)

 half3 DiffuseAndSpecularFromMetallic (half3 albedo, half metallic, out half3 specColor, out half oneMinusReflectivity)
{
    specColor = mix (unity_ColorSpaceDielectricSpec.rgb, albedo, metallic);
    oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
    return albedo * oneMinusReflectivity;
}

half4 LightingStandard (SurfaceOutputStandard s, float3 viewDir, UnityLight lg)
{
    half oneMinusReflectivity;
    half3 specColor;
    s.Albedo = DiffuseAndSpecularFromMetallic (s.Albedo, s.Metallic, /*out*/ specColor, /*out*/ oneMinusReflectivity);

  UnityIndirect gis;
    half4 c = BRDF1_Unity_PBS (s.Albedo, specColor, oneMinusReflectivity, s.Smoothness, s.Normal, viewDir, lg, gis);


    half4 emission = half4(s.Emission + c.rgb, 1);
    return emission;
}


/*
struct SurfaceOutputStandard
{
    vec3 Albedo;      // base (diffuse or specular) color
    float3 Normal;      // tangent space normal, if written
    half3 Emission;
    half Metallic;      // 0=non-metal, 1=metal
    // Smoothness is the user facing name, it should be perceptual smoothness but user should not have to deal with it.
    // Everywhere in the code you meet smoothness it is perceptual smoothness
    half Smoothness;    // 0=rough, 1=smooth
    half Occlusion;     // occlusion (default 1)
    float Alpha;        // alpha for transparencies
};

*/

void main()
{
  UnityLight light;
  light.color = vec3(1.0, 1.0, 1.0) * 6.0;
  light.dir = normalize(vec3(1.0, -1.0, -1.0));
 
  SurfaceOutputStandard e;
  e.Albedo = vec3(1.0, 0.01, 0.01);
  e.Normal = -fNormal;
  e.Metallic = 0.1;
  e.Smoothness = 0.5;
  e.Occlusion = 1.0;
  gl_FragColor = vec4(pow(LightingStandard(e, vec3(0.0, 0.0, 1.0), light), vec4(1.0/2.2)));
}