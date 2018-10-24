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
uniform vec2	  _Curve;
uniform float	  _PrefilterOffs;
uniform float	  _SampleScale;
uniform float	  _Intensity;

void main()
{
	
	vec2 texelSize = vec2(1.0) / textureSize(_MainTex, 0);
	vec4 d = texelSize.xyxy * vec4(-1, -1, +1, +1);

    vec3 s = texture(_MainTex, uv + d.xy).rgb;
    s += texture(_MainTex, uv + d.zy).rgb;
    s += texture(_MainTex, uv + d.xw).rgb;
    s += texture(_MainTex, uv + d.zw).rgb;

    FragColor = vec4( s * (1.0 / 4), 1.0);
}