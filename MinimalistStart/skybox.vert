#version 130
#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 VertexPosition;
uniform mat4 projection;
uniform mat4 view;


#define half float
#define float3 vec3
#define float4 vec4
#define half3 vec3
#define half4 vec4
#define fixed3 vec3
#define fixed4 vec4

half _Exposure = 1.3;     // HDR exposure
half3 _GroundColor = pow(vec3(.369, .349, .341), vec3(2.2));
half _SunSize = 0.04;
half _SunSizeConvergence = 5.0;
half3 _SkyTint = pow(vec3(.5, .5, .5), vec3(2.2));
half _AtmosphereThickness = 1.0;
vec3 _WorldSpaceLightPos0  = normalize(vec3(-1.0, 1.0, -1.0)) ;
vec4 _LightColor0 = pow(vec4(1.0, 1.0, 1.0, 1.0), vec4(2.2));


#define unity_ColorSpaceDouble fixed4(4.59479380, 4.59479380, 4.59479380, 2.0)
#define GAMMA 2.2

#define COLOR_2_LINEAR(color) color
#define LINEAR_2_LINEAR(color) color

vec3 COLOR_2_GAMMA(vec3 color)
{
  if(unity_ColorSpaceDouble.r>2.0)
    return pow(color, vec3(1.0/GAMMA));
  else
    return color;
}

const float3 kDefaultScatteringWavelength = float3(.65, .57, .475);
const float3 kVariableRangeForScatteringWavelength = float3(.15, .15, .15);

#define OUTER_RADIUS 1.025
const float kOuterRadius = OUTER_RADIUS;
const float kOuterRadius2 = OUTER_RADIUS*OUTER_RADIUS;
const float kInnerRadius = 1.0;
const float kInnerRadius2 = 1.0;

const float kCameraHeight = 0.0001;

#define kRAYLEIGH (mix(0.0, 0.0025, pow(_AtmosphereThickness,2.5)))      // Rayleigh constant
#define kMIE 0.0010             // Mie constant
#define kSUN_BRIGHTNESS 20.0    // Sun brightness

#define kMAX_SCATTER 50.0 // Maximum scattering value, to prevent math overflows on Adrenos

const half kHDSundiskIntensityFactor = 15.0;
const half kSimpleSundiskIntensityFactor = 27.0;

const half kSunScale = 400.0 * kSUN_BRIGHTNESS;
const float kKmESun = kMIE * kSUN_BRIGHTNESS;
const float kKm4PI = kMIE * 4.0 * 3.14159265;
const float kScale = 1.0 / (OUTER_RADIUS - 1.0);
const float kScaleDepth = 0.25;
const float kScaleOverScaleDepth = (1.0 / (OUTER_RADIUS - 1.0)) / 0.25;
const float kSamples = 2.0; // THIS IS UNROLLED MANUALLY, DON'T TOUCH

#define MIE_G (-0.990)
#define MIE_G2 0.9801
#define SKY_GROUND_THRESHOLD 0.02

// sun disk rendering:
// no sun disk - the fastest option
#define SKYBOX_SUNDISK_NONE 0
// simplistic sun disk - without mie phase function
#define SKYBOX_SUNDISK_SIMPLE 1
// full calculation - uses mie phase function
#define SKYBOX_SUNDISK_HQ 2


#define SKYBOX_SUNDISK SKYBOX_SUNDISK_HQ

#ifndef SKYBOX_COLOR_IN_TARGET_COLOR_SPACE
  #if defined(SHADER_API_MOBILE)
      #define SKYBOX_COLOR_IN_TARGET_COLOR_SPACE 1
  #else
      #define SKYBOX_COLOR_IN_TARGET_COLOR_SPACE 0
  #endif
#endif

half getRayleighPhase(half eyeCos2)
{
    return 0.75 + 0.75*eyeCos2;
}

vec3 getRayleighPhase(vec3 light, vec3 ray)
{
    half eyeCos = dot(light, ray);
    return vec3(getRayleighPhase(eyeCos * eyeCos));
}

float scale(float inCos)
{
    float x = 1.0 - inCos;
#if defined(SHADER_API_N3DS)
    // The polynomial expansion here generates too many swizzle instructions for the 3DS vertex assembler
    // Approximate by removing x^1 and x^2
    return 0.25 * exp(-0.00287 + x*x*x*(-6.80 + x*5.25));
#else
    return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
#endif
}

float saturate(float v)
{
  return clamp(v, 0.0, 1.0);
}
vec3 saturate(vec3 v)
{
  return clamp(v, vec3(0.0), vec3(1.0));
}

   out  vec4 OUT_pos;
   out  vec3 OUT_vertex;
    out vec3 OUT_radDir;
   out  float OUT_skyGroundFactor;
    out vec3   OUT_groundColor;
    out vec3  OUT_skyColor;    
    out vec3   OUT_sunColor;




void main()
{
    TexCoords = aPos;
	VertexPosition = aPos * 1000;
     
	gl_Position = projection * mat4(mat3(view)) * vec4(aPos * 1000, 1.0); 


    float3 kSkyTintInGammaSpace = COLOR_2_GAMMA(_SkyTint); // convert tint from Linear back to Gamma
    float3 kScatteringWavelength = mix (
        kDefaultScatteringWavelength-kVariableRangeForScatteringWavelength,
        kDefaultScatteringWavelength+kVariableRangeForScatteringWavelength,
        half3(1,1,1) - kSkyTintInGammaSpace); // using Tint in sRGB gamma allows for more visually linear interpolation and to keep (.5) at (128, gray in sRGB) point
    float3 kInvWavelength = vec3(1.0) / pow(kScatteringWavelength, vec3(4));

    float kKrESun = kRAYLEIGH * kSUN_BRIGHTNESS;
    float kKr4PI = kRAYLEIGH * 4.0 * 3.14159265;

    float3 cameraPos = float3(0,kInnerRadius + kCameraHeight,0);    // The camera's current position

    // Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
    float3 eyeRay = normalize(aPos);

    float far = 0.0;
    half3 cIn, cOut;

    if(eyeRay.y >= 0.0)
    {
        // Sky
        // Calculate the length of the "atmosphere"
        far = sqrt(kOuterRadius2 + kInnerRadius2 * eyeRay.y * eyeRay.y - kInnerRadius2) - kInnerRadius * eyeRay.y;

        float3 pos = cameraPos + far * eyeRay;

        // Calculate the ray's starting position, then calculate its scattering offset
        float height = kInnerRadius + kCameraHeight;
        float depth = exp(kScaleOverScaleDepth * (-kCameraHeight));
        float startAngle = dot(eyeRay, cameraPos) / height;
        float startOffset = depth*scale(startAngle);


        // Initialize the scattering loop variables
        float sampleLength = far / kSamples;
        float scaledLength = sampleLength * kScale;
        float3 sampleRay = eyeRay * sampleLength;
        float3 samplePoint = cameraPos + sampleRay * 0.5;

        // Now loop through the sample rays
        float3 frontColor = float3(0.0, 0.0, 0.0);
        // Weird workaround: WP8 and desktop FL_9_3 do not like the for loop here
        // (but an almost identical loop is perfectly fine in the ground calculations below)
        // Just unrolling this manually seems to make everything fine again.
//              for(int i=0; i<int(kSamples); i++)
        {
            float height = length(samplePoint);
            float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
            float lightAngle = dot(_WorldSpaceLightPos0.xyz, samplePoint) / height;
            float cameraAngle = dot(eyeRay, samplePoint) / height;
            float scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));
            float3 attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));

            frontColor += attenuate * (depth * scaledLength);
            samplePoint += sampleRay;
        }
        {
            float height = length(samplePoint);
            float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
            float lightAngle = dot(_WorldSpaceLightPos0.xyz, samplePoint) / height;
            float cameraAngle = dot(eyeRay, samplePoint) / height;
            float scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));
            float3 attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));

            frontColor += attenuate * (depth * scaledLength);
            samplePoint += sampleRay;
        }



        // Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
        cIn = frontColor * (kInvWavelength * kKrESun);
        cOut = frontColor * kKmESun;
    }
    else
    {
        // Ground
        far = (-kCameraHeight) / (min(-0.001, eyeRay.y));

        float3 pos = cameraPos + far * eyeRay;

        // Calculate the ray's starting position, then calculate its scattering offset
        float depth = exp((-kCameraHeight) * (1.0/kScaleDepth));
        float cameraAngle = dot(-eyeRay, pos);
        float lightAngle = dot(_WorldSpaceLightPos0.xyz, pos);
        float cameraScale = scale(cameraAngle);
        float lightScale = scale(lightAngle);
        float cameraOffset = depth*cameraScale;
        float temp = (lightScale + cameraScale);

        // Initialize the scattering loop variables
        float sampleLength = far / kSamples;
        float scaledLength = sampleLength * kScale;
        float3 sampleRay = eyeRay * sampleLength;
        float3 samplePoint = cameraPos + sampleRay * 0.5;

        // Now loop through the sample rays
        float3 frontColor = float3(0.0, 0.0, 0.0);
        float3 attenuate;
//              for(int i=0; i<int(kSamples); i++) // Loop removed because we kept hitting SM2.0 temp variable limits. Doesn't affect the image too much.
        {
            float height = length(samplePoint);
            float depth = exp(kScaleOverScaleDepth * (kInnerRadius - height));
            float scatter = depth*temp - cameraOffset;
            attenuate = exp(-clamp(scatter, 0.0, kMAX_SCATTER) * (kInvWavelength * kKr4PI + kKm4PI));
            frontColor += attenuate * (depth * scaledLength);
            samplePoint += sampleRay;
        }

        cIn = frontColor * (kInvWavelength * kKrESun + kKmESun);
        cOut = clamp(attenuate, 0.0, 1.0);
    }

#if SKYBOX_SUNDISK == SKYBOX_SUNDISK_HQ
    OUT_vertex          = -aPos;
#elif SKYBOX_SUNDISK == SKYBOX_SUNDISK_SIMPLE
    OUT.rayDir          = half3(-eyeRay);
#else
    OUT.skyGroundFactor = -eyeRay.y / SKY_GROUND_THRESHOLD;
#endif


    // if we want to calculate color in vprog:
    // 1. in case of linear: multiply by _Exposure in here (even in case of lerp it will be common multiplier, so we can skip mul in fshader)
    // 2. in case of gamma and SKYBOX_COLOR_IN_TARGET_COLOR_SPACE: do sqrt right away instead of doing that in fshader
          
    OUT_groundColor = _Exposure * (cIn + COLOR_2_LINEAR(_GroundColor) * cOut);
    OUT_skyColor    = _Exposure * (cIn * getRayleighPhase(_WorldSpaceLightPos0, -eyeRay));

#if SKYBOX_SUNDISK != SKYBOX_SUNDISK_NONE
    // The sun should have a stable intensity in its course in the sky. Moreover it should match the highlight of a purely specular material.
    // This matching was done using the standard shader BRDF1 on the 5/31/2017
    // Finally we want the sun to be always bright even in LDR thus the normalization of the lightColor for low intensity.
          
    half lightColorIntensity = clamp(length(_LightColor0.xyz), 0.25, 1.0);
    #if SKYBOX_SUNDISK == SKYBOX_SUNDISK_SIMPLE
        OUT.sunColor    = kSimpleSundiskIntensityFactor * saturate(cOut * kSunScale) * _LightColor0.xyz / lightColorIntensity;
    #else // SKYBOX_SUNDISK_HQ
        OUT_sunColor    = kHDSundiskIntensityFactor * vec3(saturate(cOut)) * _LightColor0.xyz / lightColorIntensity;
    #endif

#endif
}  
