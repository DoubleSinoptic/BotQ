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



vec3 UpsampleFilter(vec2 uv)
{
	vec2 texelSize = vec2(1.0) / textureSize(_MainTex, 0);
    vec4 d = texelSize.xyxy * vec4(1, 1, -1, 0) * _SampleScale;

    vec3 s = texture(_MainTex, uv - d.xy).rgb;
    s += texture(_MainTex, uv - d.wy).rgb * 2;
    s += texture(_MainTex, uv - d.zy).rgb;

    s += texture(_MainTex, uv + d.zw).rgb * 2;
    s += texture(_MainTex, uv       ).rgb * 4;
    s += texture(_MainTex, uv + d.xw).rgb * 2;

    s += texture(_MainTex, uv + d.zy).rgb;
    s += texture(_MainTex, uv + d.wy).rgb * 2;
    s += texture(_MainTex, uv + d.xy).rgb;

    return s * (1.0 / 16);
}

void main()
{
	vec2 texelSize = vec2(1.0) / textureSize(_MainTex, 0);

    vec4 base = texture(_BaseTex, uv + texelSize * _PrefilterOffs);
    vec3 blur = UpsampleFilter(uv);

 
    vec3 cout = base.rgb + blur * _Intensity;

  
    FragColor = vec4(cout, base.a);
}