#pragma once

#include "Gl3dCore.h"

enum class ShaderType 
{
	VertexShader, FragmentShader, PixelShader
};

class GL3DC_EXPORT Gl3dShaderException : public std::exception
{
	const char* mErrorString;
public:
	Gl3dShaderException(const char* errorStirng) : mErrorString(errorStirng) {}
	const char* what() const noexcept override;
};

class ShaderImpl;
class GL3DC_EXPORT Gl3dShader
{
	ShaderImpl* mImpl = nullptr;
public:
	Gl3dShader();
	~Gl3dShader();

	void Build(const char* asciitext, ShaderType type);
	void Relink();
	unsigned int GetObject() const;
	int UniformLocation(const char* ifName) const;
	int UniformLocationExt(const char* ifName, int id) const;
	int MapLocation(const char* ifName, int id) const;
};

