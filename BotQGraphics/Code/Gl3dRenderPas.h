#pragma once

#include "Gl3dCore.h"

class Gl3dShader;
class Gl3dFrameBufferBase;
class Gl3dTexture;

#ifdef __USE_FAST_UNIF
#define FastUniform(...) OUniform(__COUNTER__, __VA_ARGS__)  
#else
#define FastUniform(...) Uniform(__VA_ARGS__)  
#endif

enum class Gl3dDepth 
{
	Disable = 0,
	LessEqual = 1,
	Greater = 2,
	GreaterEqual = 3,
	Less = 4
};

enum class Gl3dCullFace
{
	Disable = 0,
	Front = 1,
	Back = 2
};

enum class Gl3dBlending
{
	Disable = 0
};

struct Gl3dViewport
{
	int x;
	int y;
	int w;
	int h;
};

enum class Gl3dClear
{
	None = 0,
	Depth = 1,
	Color = 2,
	DepthColor = 3
};

struct Gl3dClearColor4f
{
	float r,  g,  b,  a;
};

struct Gl3dRenderPassDesc
{
	Gl3dDepth    depth;
	Gl3dCullFace cullFace;
	Gl3dBlending blending;
	Gl3dViewport viewport;
	Gl3dClear    clear;
	Gl3dClearColor4f clearColor;
	float        clearDepth;
};

class GL3DC_EXPORT Gl3dRenderPas
{
public:
	Gl3dRenderPas(Gl3dShader* shader, Gl3dFrameBufferBase* framebuffer, Gl3dRenderPassDesc* desc = nullptr);
	~Gl3dRenderPas();

	void OUniform(int cap, const char* uniform, Gl3dTexture* texture) const;
	void OUniform(int cap, const char* uniform, int s) const;
	void OUniform(int cap, const char* uniform, bool s) const;
	void OUniform(int cap, const char* uniform, float s) const;

	void OUniform2f(int cap, const char* uniform, const float* s) const;
	void OUniform3f(int cap, const char* uniform, const float* s) const;
	void OUniform4f(int cap, const char* uniform, const float* s) const;
	void OUniform16f(int cap, const char* uniform, const float* s) const;


	void Uniform(const char* uniform, Gl3dTexture* texture) const;
	void Uniform(const char* uniform, int s) const;
	void Uniform(const char* uniform, bool s) const;
	void Uniform(const char* uniform, float s) const;

	void Uniform2f(const char* uniform, const float* s) const;
	void Uniform3f(const char* uniform, const float* s) const;
	void Uniform4f(const char* uniform, const float* s) const;
	void Uniform16f(const char* uniform, const float* s) const;


	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 16) && IsClass<T>::Value, void>::Type
		OUniform(int cap, const char* uniform, const T& val)
	{
		OUniform16f(cap, uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 4) && IsClass<T>::Value, void>::Type
		OUniform(int cap, const char* uniform, const T& val)
	{
		OUniform4f(cap, uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 3) && IsClass<T>::Value, void>::Type
		OUniform(int cap, const char* uniform, const T& val)
	{
		OUniform3f(cap, uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<(sizeof(T) == (sizeof(float) * 2)) && IsClass<T>::Value, void>::Type
		OUniform(int cap, const char* uniform, const T& val)
	{
		OUniform2f(cap, uniform, reinterpret_cast<const float*>(&val));
	}



	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 16) && IsClass<T>::Value, void>::Type
		Uniform(const char* uniform, const T& val)
	{
		Uniform16f(uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 4) && IsClass<T>::Value, void>::Type
		Uniform(const char* uniform, const T& val)
	{
		Uniform4f(uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<sizeof(T) == (sizeof(float) * 3) && IsClass<T>::Value, void>::Type
		Uniform(const char* uniform, const T& val)
	{
		Uniform3f(uniform, reinterpret_cast<const float*>(&val));
	}

	template<typename T>
	typename Gl3dEnableIf<(sizeof(T) == (sizeof(float) * 2)) && IsClass<T>::Value, void>::Type
		Uniform(const char* uniform, const T& val)
	{
		Uniform2f(uniform, reinterpret_cast<const float*>(&val));
	}
private:
	Gl3dShader* mShader = nullptr;
	Gl3dFrameBufferBase* mFramebuffer = nullptr;
};

class GL3DC_EXPORT Gl3dPipeLine 
{
public:
	Gl3dPipeLine() {}
	virtual ~Gl3dPipeLine() {}

	virtual void Frabricate() = 0;

	Gl3dTexture* Output();
};