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


class GL3DC_EXPORT Gl3dRenderPas
{
public:
	Gl3dRenderPas(Gl3dShader* shader, Gl3dFrameBufferBase* framebuffer);
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