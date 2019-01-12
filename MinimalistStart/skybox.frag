
#version 400
#extension GL_ARB_explicit_attrib_location : enable
layout ( location = 0 ) out vec4 OutFragColor1;
layout ( location = 1 ) out vec3 OutPosition1;
layout ( location = 2 ) out vec3 OutNormal1;
layout ( location = 3 ) out vec3 OutMaterial1;

in vec3 TexCoords;
in vec3 VertexPosition;

uniform samplerCube skybox;




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

   in  vec4 OUT_pos;
   in  vec3 OUT_vertex;
    in vec3 OUT_radDir;
   in  float OUT_skyGroundFactor;
    in vec3   OUT_groundColor;
    in vec3  OUT_skyColor;    
    in vec3   OUT_sunColor;


	
half getMiePhase(half eyeCos, half eyeCos2)
        {
            half temp = 1.0 + MIE_G2 - 2.0 * MIE_G * eyeCos;
            temp = pow(temp, pow(_SunSize,0.65) * 10.0);
            temp = max(temp,1.0e-4); // prevent division by zero, esp. in half precision
            temp = 1.5 * ((1.0 - MIE_G2) / (2.0 + MIE_G2)) * (1.0 + eyeCos2) / temp;
            #if defined(UNITY_COLORSPACE_GAMMA) && SKYBOX_COLOR_IN_TARGET_COLOR_SPACE
                temp = pow(temp, .454545);
            #endif
            return temp;
        }
        
   half calcSunAttenuation(half3 lightPos, half3 ray)
        {
        #if SKYBOX_SUNDISK == SKYBOX_SUNDISK_SIMPLE
            half3 delta = lightPos - ray;
            half dist = length(delta);
            half spot = 1.0 - smoothstep(0.0, _SunSize, dist);
            return spot * spot;
        #else // SKYBOX_SUNDISK_HQ
            half focusedEyeCos = pow(saturate(dot(lightPos, ray)), _SunSizeConvergence);
            return getMiePhase(-focusedEyeCos, focusedEyeCos * focusedEyeCos);
        #endif
        }


uniform mat4 view;

void main()
{    
	OutPosition1 = VertexPosition;
	OutNormal1 = vec3(0.0);
    //FragColor = vec4(texture(skybox, TexCoords).rgb, 1.0);
	OutMaterial1 = vec3(0.0);
	

	half3 col = half3(0.0, 0.0, 0.0);

    // if y > 1 [eyeRay.y < -SKY_GROUND_THRESHOLD] - ground
    // if y >= 0 and < 1 [eyeRay.y <= 0 and > -SKY_GROUND_THRESHOLD] - horizon
    // if y < 0 [eyeRay.y > 0] - sky
    #if SKYBOX_SUNDISK == SKYBOX_SUNDISK_HQ
        half3 ray = normalize(OUT_vertex);
        half y = ray.y / SKY_GROUND_THRESHOLD;
    #elif SKYBOX_SUNDISK == SKYBOX_SUNDISK_SIMPLE
        half3 ray = IN.rayDir.xyz;
        half y = ray.y / SKY_GROUND_THRESHOLD;
    #else
        half y = IN.skyGroundFactor;
    #endif
   
        // if we did precalculate color in vprog: just do lerp between them
        col = mix(OUT_skyColor, OUT_groundColor, saturate(y));
  
    #if SKYBOX_SUNDISK != SKYBOX_SUNDISK_NONE
        if(y < 0.0)
        {
            col += OUT_sunColor * calcSunAttenuation(_WorldSpaceLightPos0.xyz, -ray);
        }
    #endif
  
  
   OutFragColor1 = vec4(col, 1.0);

}