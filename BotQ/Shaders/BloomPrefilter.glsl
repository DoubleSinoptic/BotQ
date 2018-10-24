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


float Brightness(vec3 c)
{
    return max(max(c.r, c.g), c.b);
}

vec3 Median(vec3 a, vec3 b, vec3 c)
{
    return a + b + c - min(min(a, b), c) - max(max(a, b), c);
}

void main()
{
	vec3 m = texture(_MainTex, uv).rgb;


	m = pow(m, vec3(1/2.2));

	float br = Brightness(m);

	float rq = clamp(br - _Curve.x, 0, _Curve.y);
    rq = _Curve.z * rq * rq;

	m *= max(rq, br - _Threshold) / max(br, 1e-5);

	FragColor = vec4(m, 1.0);
}