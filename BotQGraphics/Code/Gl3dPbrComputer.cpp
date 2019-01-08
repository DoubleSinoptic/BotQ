#include "Gl3dPbrComputer.h"
#include "Gl3dTexture.h"
#include "Gl3dShader.h"
#include "Gl3dRenderPas.h"
#include "Gl3dDevice.h"
#include "glad.h"
#undef GetObject

#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


template<typename Funct>
struct _Guard 
{
	void Disable() { enable = false; }
	bool enable = true;
	Funct f;
	_Guard(const Funct& x) : f(x) {}
	~_Guard()
	{if(enable) f();}
};

template<typename T>
_Guard<T> GReg(const T& x)
{
	return x;
}

struct Gl3dPbrComputerImpl 
{
	Gl3dPbrComputerImpl() 
	{

	}

	~Gl3dPbrComputerImpl() 
	{

	}
};

Gl3dPbrComputer::Gl3dPbrComputer() :
	mImpl(new Gl3dPbrComputerImpl())
{

}

void testing() 
{
	


}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

const char* cubemap_vs = "#version 330 core\nlayout (location = 0) in vec3 aPos;\n\nout vec3 WorldPos;\n\nuniform mat4 projection;\nuniform mat4 view;\n\nvoid main()\n{\n    WorldPos = aPos;  \n    gl_Position =  projection * view * vec4(WorldPos, 1.0);\n}";

void Gl3dPbrComputer::RenderMaps(Gl3dTexture * hdrSphereMap)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// set depth function to less than AND equal for skybox depth trick.
	glDepthFunc(GL_LEQUAL);
	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	Gl3dShader equirectangularToCubemapShader;
	equirectangularToCubemapShader.Build(cubemap_vs,
		ShaderType::VertexShader);
	equirectangularToCubemapShader.Build("#version 330 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform sampler2D equirectangularMap;\n\nconst vec2 invAtan = vec2(0.1591, 0.3183);\nvec2 SampleSphericalMap(vec3 v)\n{\n    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n    uv *= invAtan;\n    uv += 0.5;\n    return uv;\n}\n\nvoid main()\n{\t\t\n    vec2 uv = SampleSphericalMap(normalize(WorldPos));\n    vec3 color = clamp(texture(equirectangularMap, uv).rgb, vec3(0.0), vec3(1000.0));\n    \n    FragColor = vec4(color, 1.0);\n}",
		ShaderType::FragmentShader);
	equirectangularToCubemapShader.Relink();
	
	Gl3dShader irradianceShader;
	irradianceShader.Build(cubemap_vs,
		ShaderType::VertexShader);
	irradianceShader.Build("#version 330 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform samplerCube environmentMap;\n\nconst float PI = 3.14159265359;\n\nvoid main()\n{\t\t\n    vec3 N = normalize(WorldPos);\n\n    vec3 irradiance = vec3(0.0);   \n    \n    // tangent space calculation from origin point\n    vec3 up    = vec3(0.0, 1.0, 0.0);\n    vec3 right = cross(up, N);\n    up            = cross(N, right);\n       \n    float sampleDelta = 0.025;\n    float nrSamples = 0.0f;\n    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)\n    {\n        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)\n        {\n            // spherical to cartesian (in tangent space)\n            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));\n            // tangent space to world\n            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; \n\n            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);\n            nrSamples++;\n        }\n    }\n    irradiance = PI * irradiance * (1.0 / float(nrSamples));\n    \n    FragColor = vec4(irradiance, 1.0);\n}",
		ShaderType::FragmentShader);
	irradianceShader.Relink();

	Gl3dShader prefilterShader;
	prefilterShader.Build(cubemap_vs,
		ShaderType::VertexShader);
	prefilterShader.Build("#version 330 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform samplerCube environmentMap;\nuniform float roughness;\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\nfloat DistributionGGX(vec3 N, vec3 H, float roughness)\n{\n    float a = roughness*roughness;\n    float a2 = a*a;\n    float NdotH = max(dot(N, H), 0.0);\n    float NdotH2 = NdotH*NdotH;\n\n    float nom   = a2;\n    float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n    denom = PI * denom * denom;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\n// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html\n// efficient VanDerCorpus calculation.\nfloat RadicalInverse_VdC(uint bits) \n{\n     bits = (bits << 16u) | (bits >> 16u);\n     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n     return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n}\n// ----------------------------------------------------------------------------\nvec2 Hammersley(uint i, uint N)\n{\n\treturn vec2(float(i)/float(N), RadicalInverse_VdC(i));\n}\n// ----------------------------------------------------------------------------\nvec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)\n{\n\tfloat a = roughness*roughness;\n\t\n\tfloat phi = 2.0 * PI * Xi.x;\n\tfloat cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));\n\tfloat sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n\t\n\t// from spherical coordinates to cartesian coordinates - halfway vector\n\tvec3 H;\n\tH.x = cos(phi) * sinTheta;\n\tH.y = sin(phi) * sinTheta;\n\tH.z = cosTheta;\n\t\n\t// from tangent-space H vector to world-space sample vector\n\tvec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\tvec3 tangent   = normalize(cross(up, N));\n\tvec3 bitangent = cross(N, tangent);\n\t\n\tvec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n\treturn normalize(sampleVec);\n}\n// ----------------------------------------------------------------------------\nvoid main()\n{\t\t\n    vec3 N = normalize(WorldPos);\n    \n    // make the simplyfying assumption that V equals R equals the normal \n    vec3 R = N;\n    vec3 V = R;\n\n    const uint SAMPLE_COUNT = 1024u;\n    vec3 prefilteredColor = vec3(0.0);\n    float totalWeight = 0.0;\n    \n    for(uint i = 0u; i < SAMPLE_COUNT; ++i)\n    {\n        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).\n        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n        vec3 H = ImportanceSampleGGX(Xi, N, roughness);\n        vec3 L  = normalize(2.0 * dot(V, H) * H - V);\n\n        float NdotL = max(dot(N, L), 0.0);\n        if(NdotL > 0.0)\n        {\n            // sample from the environment's mip level based on roughness/pdf\n            float D   = DistributionGGX(N, H, roughness);\n            float NdotH = max(dot(N, H), 0.0);\n            float HdotV = max(dot(H, V), 0.0);\n            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; \n\n            float resolution = 512.0; // resolution of source cubemap (per face)\n            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);\n            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);\n\n            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); \n            \n            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;\n            totalWeight      += NdotL;\n        }\n    }\n\n    prefilteredColor = prefilteredColor / totalWeight;\n\n    FragColor = vec4(prefilteredColor, 1.0);\n}",
		ShaderType::FragmentShader);
	prefilterShader.Relink();

	Gl3dShader brdfShader;
	brdfShader.Build("#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoords;\n\nout vec2 TexCoords;\n\nvoid main()\n{\n    TexCoords = aTexCoords;\n\tgl_Position = vec4(aPos, 1.0);\n}",
		ShaderType::VertexShader);
	brdfShader.Build("#version 330 core\nout vec2 FragColor;\nin vec2 TexCoords;\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\n// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html\n// efficient VanDerCorpus calculation.\nfloat RadicalInverse_VdC(uint bits) \n{\n     bits = (bits << 16u) | (bits >> 16u);\n     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n     return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n}\n// ----------------------------------------------------------------------------\nvec2 Hammersley(uint i, uint N)\n{\n\treturn vec2(float(i)/float(N), RadicalInverse_VdC(i));\n}\n// ----------------------------------------------------------------------------\nvec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)\n{\n\tfloat a = roughness*roughness;\n\t\n\tfloat phi = 2.0 * PI * Xi.x;\n\tfloat cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));\n\tfloat sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n\t\n\t// from spherical coordinates to cartesian coordinates - halfway vector\n\tvec3 H;\n\tH.x = cos(phi) * sinTheta;\n\tH.y = sin(phi) * sinTheta;\n\tH.z = cosTheta;\n\t\n\t// from tangent-space H vector to world-space sample vector\n\tvec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\tvec3 tangent   = normalize(cross(up, N));\n\tvec3 bitangent = cross(N, tangent);\n\t\n\tvec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n\treturn normalize(sampleVec);\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySchlickGGX(float NdotV, float roughness)\n{\n    // note that we use a different k for IBL\n    float a = roughness;\n    float k = (a * a) / 2.0;\n\n    float nom   = NdotV;\n    float denom = NdotV * (1.0 - k) + k;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)\n{\n    float NdotV = max(dot(N, V), 0.0);\n    float NdotL = max(dot(N, L), 0.0);\n    float ggx2 = GeometrySchlickGGX(NdotV, roughness);\n    float ggx1 = GeometrySchlickGGX(NdotL, roughness);\n\n    return ggx1 * ggx2;\n}\n// ----------------------------------------------------------------------------\nvec2 IntegrateBRDF(float NdotV, float roughness)\n{\n    vec3 V;\n    V.x = sqrt(1.0 - NdotV*NdotV);\n    V.y = 0.0;\n    V.z = NdotV;\n\n    float A = 0.0;\n    float B = 0.0; \n\n    vec3 N = vec3(0.0, 0.0, 1.0);\n    \n    const uint SAMPLE_COUNT = 1024u;\n    for(uint i = 0u; i < SAMPLE_COUNT; ++i)\n    {\n        // generates a sample vector that's biased towards the\n        // preferred alignment direction (importance sampling).\n        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n        vec3 H = ImportanceSampleGGX(Xi, N, roughness);\n        vec3 L = normalize(2.0 * dot(V, H) * H - V);\n\n        float NdotL = max(L.z, 0.0);\n        float NdotH = max(H.z, 0.0);\n        float VdotH = max(dot(V, H), 0.0);\n\n        if(NdotL > 0.0)\n        {\n            float G = GeometrySmith(N, V, L, roughness);\n            float G_Vis = (G * VdotH) / (NdotH * NdotV);\n            float Fc = pow(1.0 - VdotH, 5.0);\n\n            A += (1.0 - Fc) * G_Vis;\n            B += Fc * G_Vis;\n        }\n    }\n    A /= float(SAMPLE_COUNT);\n    B /= float(SAMPLE_COUNT);\n    return vec2(A, B);\n}\n// ----------------------------------------------------------------------------\nvoid main() \n{\n    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);\n    FragColor = integratedBRDF;\n}",
		ShaderType::FragmentShader);
	brdfShader.Relink();

	if (IrradianceMap || PrefilterMap || BrdfLut)
		throw Gl3dCoreException("ibl maps prerendered");

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	glViewport(0, 0, 1024, 1024);

	float color1[] = { 
		1.0f, 0.0f , 0.0f, 
		0.0f, 1.0f , 0.0f,
		0.0f, 0.0f , 1.0f,
		0.0f, 1.0f , 1.0f,
		1.0f, 1.0f , 0.0f,
		1.0f, 0.0f , 1.0f 
	};
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glm::mat4 captureProjection = glm::perspectiveLH(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	
	//pass 1
	
	Gl3dRenderPas pass1(&equirectangularToCubemapShader, nullptr);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	pass1.Uniform("equirectangularMap", hdrSphereMap);
	pass1.Uniform("projection", captureProjection);
	

	for (unsigned int i = 0; i < 6; ++i)
	{
		pass1.Uniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
	
		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			g3dlog("framebuffer not attached: %d", status);

		glViewport(0, 0, 1024, 1024);
		glClearColor(0.0, 0.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	EnvaromentSkybox = new Gl3dTexture(envCubemap, GL_TEXTURE_CUBE_MAP);
	//pass 2

	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	
	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	Gl3dRenderPas pass2 = Gl3dRenderPas(&irradianceShader, nullptr);
	pass2.Uniform("environmentMap", EnvaromentSkybox);
	pass2.Uniform("projection", captureProjection);
	

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		pass2.Uniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	IrradianceMap = new Gl3dTexture(irradianceMap, GL_TEXTURE_CUBE_MAP);
	
	//pass 3

	// pbr: create a pre - filter cubemap, and re - scale capture FBO to pre - filter scale.
		// --------------------------------------------------------------------------------
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	Gl3dRenderPas pass = Gl3dRenderPas(&prefilterShader, nullptr);
	pass.Uniform("environmentMap", EnvaromentSkybox);
	pass.Uniform("projection", captureProjection);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 11;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		pass.Uniform("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			pass.Uniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	PrefilterMap = new Gl3dTexture(prefilterMap, GL_TEXTURE_CUBE_MAP);
	//tmp = new Gl3dTexture(prefilterMap, GL_TEXTURE_CUBE_MAP);
	/*
	
	glEnable(GL_CULL_FACE);
	//pass 4
	Gl3dRenderPas pass5 = Gl3dRenderPas(&brdfShader, nullptr);
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	unsigned int gac;
	glGenFramebuffers(1, &gac);
	glBindFramebuffer(GL_FRAMEBUFFER, gac);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);

	glClearColor(0.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	BrdfLut = new Gl3dTexture(brdfLUTTexture, GL_TEXTURE_2D);
	
	*/
	RenderBrdfLut();
}

void Gl3dPbrComputer::RenderBrdfLut()
{
	Gl3dShader brdfShader;
	brdfShader.Build("#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoords;\n\nout vec2 TexCoords;\n\nvoid main()\n{\n    TexCoords = aTexCoords;\n\tgl_Position = vec4(aPos, 1.0);\n}",
		ShaderType::VertexShader);
	brdfShader.Build("#version 330 core\nout vec2 FragColor;\nin vec2 TexCoords;\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\n// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html\n// efficient VanDerCorpus calculation.\nfloat RadicalInverse_VdC(uint bits) \n{\n     bits = (bits << 16u) | (bits >> 16u);\n     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n     return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n}\n// ----------------------------------------------------------------------------\nvec2 Hammersley(uint i, uint N)\n{\n\treturn vec2(float(i)/float(N), RadicalInverse_VdC(i));\n}\n// ----------------------------------------------------------------------------\nvec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)\n{\n\tfloat a = roughness*roughness;\n\t\n\tfloat phi = 2.0 * PI * Xi.x;\n\tfloat cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));\n\tfloat sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n\t\n\t// from spherical coordinates to cartesian coordinates - halfway vector\n\tvec3 H;\n\tH.x = cos(phi) * sinTheta;\n\tH.y = sin(phi) * sinTheta;\n\tH.z = cosTheta;\n\t\n\t// from tangent-space H vector to world-space sample vector\n\tvec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\tvec3 tangent   = normalize(cross(up, N));\n\tvec3 bitangent = cross(N, tangent);\n\t\n\tvec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n\treturn normalize(sampleVec);\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySchlickGGX(float NdotV, float roughness)\n{\n    // note that we use a different k for IBL\n    float a = roughness;\n    float k = (a * a) / 2.0;\n\n    float nom   = NdotV;\n    float denom = NdotV * (1.0 - k) + k;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)\n{\n    float NdotV = max(dot(N, V), 0.0);\n    float NdotL = max(dot(N, L), 0.0);\n    float ggx2 = GeometrySchlickGGX(NdotV, roughness);\n    float ggx1 = GeometrySchlickGGX(NdotL, roughness);\n\n    return ggx1 * ggx2;\n}\n// ----------------------------------------------------------------------------\nvec2 IntegrateBRDF(float NdotV, float roughness)\n{\n    vec3 V;\n    V.x = sqrt(1.0 - NdotV*NdotV);\n    V.y = 0.0;\n    V.z = NdotV;\n\n    float A = 0.0;\n    float B = 0.0; \n\n    vec3 N = vec3(0.0, 0.0, 1.0);\n    \n    const uint SAMPLE_COUNT = 1024u;\n    for(uint i = 0u; i < SAMPLE_COUNT; ++i)\n    {\n        // generates a sample vector that's biased towards the\n        // preferred alignment direction (importance sampling).\n        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n        vec3 H = ImportanceSampleGGX(Xi, N, roughness);\n        vec3 L = normalize(2.0 * dot(V, H) * H - V);\n\n        float NdotL = max(L.z, 0.0);\n        float NdotH = max(H.z, 0.0);\n        float VdotH = max(dot(V, H), 0.0);\n\n        if(NdotL > 0.0)\n        {\n            float G = GeometrySmith(N, V, L, roughness);\n            float G_Vis = (G * VdotH) / (NdotH * NdotV);\n            float Fc = pow(1.0 - VdotH, 5.0);\n\n            A += (1.0 - Fc) * G_Vis;\n            B += Fc * G_Vis;\n        }\n    }\n    A /= float(SAMPLE_COUNT);\n    B /= float(SAMPLE_COUNT);\n    return vec2(A, B);\n}\n// ----------------------------------------------------------------------------\nvoid main() \n{\n    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);\n    FragColor = integratedBRDF;\n}",
		ShaderType::FragmentShader);
	brdfShader.Relink();
	Gl3dRenderPas pass5 = Gl3dRenderPas(&brdfShader, nullptr);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);
	auto gTexture = GReg([&]()
	{
		glDeleteTextures(1, &brdfLUTTexture);
	});

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int brdfBack;
	glGenFramebuffers(1, &brdfBack);
	auto __ = GReg([&]() 
	{
		glDeleteFramebuffers(1, &brdfBack);
	});
	glBindFramebuffer(GL_FRAMEBUFFER, brdfBack);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glViewport(0, 0, 512, 512);

	glClearColor(0.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderQuad();

	BrdfLut = new Gl3dTexture(brdfLUTTexture, GL_TEXTURE_2D);
	gTexture.Disable();
}

namespace glm 
{
	float max(float a, float b)
	{
		if (a > b)
			return a;
		return b;
	}

	float min(float a, float b)
	{
		if (a < b)
			return a;
		return b;
	}
}


Gl3dPbrComputer::~Gl3dPbrComputer()
{
	if (IrradianceMap)
		delete IrradianceMap;
	if (PrefilterMap)
		delete PrefilterMap;
	if (BrdfLut)
		delete BrdfLut;
	if (EnvaromentSkybox)
		delete EnvaromentSkybox;

	if (mImpl)
		delete mImpl;
}


float PerezFunction(float A,
	float B,
	float C,
	float D,
	float E,
	float cosTheta,
	float gamma,
	float thetaS,
	float lvz)
{
	float den = (1.f + A * exp(B)) * (1.f + C * exp(D * thetaS) + E * cos(thetaS)*cos(thetaS));
	float num = (1.f + A * exp(B / cosTheta)) * (1.f + C * exp(D * gamma) + E * cos(gamma)*cos(gamma));
	return (lvz * num / den);
}
//-------------------------------------------------------------------------
glm::vec3 ComputeSunIntensity(float _sunTheta, float _sunPhi, float _turbidity)
{
	using namespace glm;
	const float     SunRadius = 0.018f;
	const float     SunFalloff = 0.022f;
	const vec2 		A_Y = vec2(0.1787, -1.4630);
	const vec2 		B_Y = vec2(-0.3554, 0.4275);
	const vec2 		C_Y = vec2(-0.0227, 5.3251);
	const vec2 		D_Y = vec2(0.1206, -2.5771);
	const vec2 		E_Y = vec2(-0.0670, 0.3703);

	const vec2 		A_x = vec2(-0.0193, -0.2592);
	const vec2 		B_x = vec2(-0.0665, 0.0008);
	const vec2 		C_x = vec2(-0.0004, 0.2125);
	const vec2 		D_x = vec2(-0.0641, -0.8989);
	const vec2 		E_x = vec2(-0.0033, 0.0452);

	const vec2 		A_y = vec2(-0.0167, -0.2608);
	const vec2 		B_y = vec2(-0.0950, 0.0092);
	const vec2 		C_y = vec2(-0.0079, 0.2102);
	const vec2 		D_y = vec2(-0.0441, -1.6537);
	const vec2 		E_y = vec2(-0.0109, 0.0529);

	const mat4x3 	Mx = mat4x3(0.00166, -0.02903, 0.11693,
		-0.00375, 0.06377, -0.21196,
		0.00209, -0.03203, 0.06052,
		0, 0.00394, 0.25886);

	const mat4x3 	My = mat4x3(0.00275, -0.04214, 0.15346,
		-0.00610, 0.08970, -0.26756,
		0.00317, -0.04153, 0.06670,
		0, 0.00516, 0.26688);

	const mat3 		XYZ2RGB = mat3(3.240479, -0.969256, 0.055648,
		-1.53715, 1.875991, -0.204043,
		-0.49853, 0.041556, 1.057311);
	//
	float cosTheta = cos(_sunTheta);
	float gamma = 0.f; 
	float thetaS = _sunTheta;

	float w = (gamma - SunRadius) / (SunFalloff - SunRadius);
	cosTheta = gamma < SunFalloff ? cos(thetaS) : cosTheta;
	gamma = gamma < SunFalloff ? 0 : gamma;
	float sunFactor = 3.50f;
	float factor = 1.f + sunFactor * smoothstep(0.f, 1.f, 1.f - w);

	float thetaS2 = thetaS * thetaS;
	float thetaS3 = thetaS * thetaS2;
	vec3  T = vec3(_turbidity*_turbidity, _turbidity, 1);
	vec4  thetas = vec4(thetaS3, thetaS2, thetaS, 1);

	float chi = (4.f / 9.0f - _turbidity / 120.f) * (3.14159265359 - 2.f * thetaS);
	float zenith_Y = (4.0453f*_turbidity - 4.9710f) * tan(chi) - 0.2155f*_turbidity + 2.4192f;
	zenith_Y *= 1000.f;

	float zenith_x = dot(T, Mx * thetas);
	float zenith_y = dot(T, My * thetas);

	vec2 t = vec2(_turbidity, 1);
	float x = PerezFunction(dot(t, A_x), dot(t, B_x), dot(t, C_x), dot(t, D_x), dot(t, E_x), cosTheta, gamma, thetaS, zenith_x);
	float y = PerezFunction(dot(t, A_y), dot(t, B_y), dot(t, C_y), dot(t, D_y), dot(t, E_y), cosTheta, gamma, thetaS, zenith_y);

	vec3 XYZ;
	XYZ.y = abs(PerezFunction(dot(t, A_Y), dot(t, B_Y), dot(t, C_Y), dot(t, D_Y), dot(t, E_Y), cosTheta, gamma, thetaS, zenith_Y))*factor;
	XYZ.x = (x / y) * XYZ.y;
	XYZ.z = ((1.f - x - y) / y) * XYZ.y;


	return XYZ2RGB * XYZ;
}



GL3DC_EXPORT void Gl3dPbrComputer::CalculateSunLuma(float * vec)
{
	vec[0] =  glm::max(glm::dot(glm::vec3(ComputeSunIntensity(0.63, 5.31, 2)), glm::vec3(0.299f, 0.587f, 0.114f)), 0.0001f);
}