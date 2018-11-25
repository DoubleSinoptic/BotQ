#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout (location = 0) in vec2 positon;


out vec2 uv;

void main()
{
	gl_Position = vec4(vec3(positon, 0.0), 1.0);    
	uv = positon * 0.5 + 0.5;
}
&
#version 130
out vec4 FragColor;

in vec2 uv;

uniform sampler2D _BaseTex;
uniform sampler2D _MainTex;
uniform float	  _Threshold;
uniform vec3	  _Curve;
uniform float	  _PrefilterOffs;
uniform float	  _SampleScale;
uniform float	  _Intensity;

vec3 LinearToGammaSpace (vec3 color)
{
    color = max(color, vec3(0, 0, 0));
    return max(1.055 * pow(color, vec3(0.416666667)) - 0.055, 0);
}

vec3 GammaToLinearSpace (vec3 color)
{
    return color * (color * (color * 0.305306011 + 0.682171111) + 0.012522878);
}

float Brightness(vec3 c)
{
	//c = pow(c, vec3(4));
    return max(max(c.r, c.g), c.b);
}

vec3 Median(vec3 a, vec3 b, vec3 c)
{
    return a + b + c - min(min(a, b), c) - max(max(a, b), c);
}


void main()
{
	float _softKnee = 0.5f;
	float _threshold = pow(0.954, 2.2);
	float knee = _threshold * _softKnee + 1e-5f;
	vec3 curve = vec3(_threshold - knee, knee * 2, 0.25f / knee);

	vec3 m = texture(_MainTex, uv).rgb;

	float br = Brightness(m);

	float rq = clamp(br - curve.x, 0, curve.y);
    rq = curve.z * rq * rq;

	m *= max(rq, br - _threshold) / max(br, 1e-5);

	FragColor = vec4(m, 1.0);


	/*

	
	vec3 m = texture(_MainTex, uv).rgb;


	float br = Brightness(m);

	float rq = clamp(br - _Curve.x, 0, _Curve.y);
    rq = _Curve.z * rq * rq;

	m *= max(rq, br - _Threshold) / max(br, 1e-5);

	FragColor = vec4(m, 1.0);
	*/
}