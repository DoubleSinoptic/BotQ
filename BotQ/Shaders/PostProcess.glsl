#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 positon;


out vec2 frag_texcoord;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	frag_texcoord = positon * 0.5 + 0.5;
}
&
#version 130
out vec4 FragColor;

in vec2 frag_texcoord;

uniform sampler2D rez_map;
uniform sampler2D god_map;
uniform sampler2D blur_map;
uniform sampler2D pre_map;
uniform sampler2D cc_map;

uniform sampler2D albedo_map;
uniform sampler2D cmp_map;

vec3 LinearToGammaSpace (vec3 color)
{
    color = max(color, vec3(0, 0, 0));
    return max(1.055 * pow(color, vec3(0.416666667)) - 0.055, 0);
}

vec3 GammaToLinearSpace (vec3 color)
{
    return color * (color * (color * 0.305306011 + 0.682171111) + 0.012522878);
}

#define MAXCOLOR 15.0 
#define COLORS 16.0
#define WIDTH 256.0 
#define HEIGHT 16.0
vec3 ColorCorrection(vec3 px, sampler2D lut)
{
	float cell = floor(px.b * MAXCOLOR);

	float half_px_x = 0.5 / WIDTH; 
    float half_px_y = 0.5 / HEIGHT;

    float x_offset = half_px_x + px.r / COLORS * (MAXCOLOR / COLORS);
    float y_offset = half_px_y + px.g * (MAXCOLOR / COLORS); 
  
    vec2 lut_pos = vec2(cell / COLORS + x_offset, y_offset); 

    vec4 graded_color = texture2D(lut, lut_pos); 
	return graded_color.rgb;
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




void main()
{
#define USE_BORDER_MASK
	vec2 coordX = frag_texcoord;
#ifdef USE_BORDER_MASK
	const float offset = 0.016;
	coordX = coordX * vec2(1.0 - offset) + vec2(offset / 2.0);
#endif
	vec3 blur = texture(blur_map, coordX).rgb;
	vec3 god = texture(god_map, coordX).rgb * 0.1;
	vec3 color = texture(rez_map, coordX).rgb;
    vec3 pre = texture(pre_map, coordX).rgb;

	float cTonemapExposureBias = 6.0f;
	float cTonemapMaxWhite = 5.0f;

	 vec3 final = 
	    Uncharted2Tonemap(max(blur * cTonemapExposureBias, 0.0)) / 
        Uncharted2Tonemap(vec3(cTonemapMaxWhite, cTonemapMaxWhite, cTonemapMaxWhite));
	//final += god;

	/*
	uniform sampler2D albedo_map;
uniform sampler2D cmp_map;
	*/
    FragColor = vec4(LinearToGammaSpace(final), 1.0);
}