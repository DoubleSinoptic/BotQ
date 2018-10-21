#include <vector>
#include <map>
#include "Gl3dShader.h"
#include "glad.h"
#undef GetObject

thread_local Gl3dShader* currentShader = nullptr;

class const_string 
{
	const char* const mStr;
public:
	constexpr const_string(const char* const str) :
		mStr(str)
	{}
	constexpr const char* const c_str() const 
	{
		return mStr;
	}
};

inline bool operator==(const_string A, const_string B)
{
	return strcmp(A.c_str(), B.c_str()) == 0;
}
inline bool operator<(const_string A, const_string B)
{
	return strcmp(A.c_str(), B.c_str()) == -1;
}
inline bool operator>(const_string A, const_string B)
{
	return strcmp(A.c_str(), B.c_str()) == 1;
}

class ShaderImpl 
{
public:

	GLuint mProgram = 0;
	std::vector<GLuint> mShaders;
	std::map<std::string, int> mTexturesInner;
	int mTexturesIter = 1;
	std::map<std::string, int> mUniforms;

	std::vector<int> extBindings;

	ShaderImpl() :
		extBindings(100, 0)
	{
		mProgram = glCreateProgram();
		g3dlog("G3D$: struct shader(%u)", mProgram);
		if (mProgram < 1)
			throw Gl3dShaderException("error of create shader program");
	}

	~ShaderImpl() 
	{
		if (mProgram > 0) 
		{
			g3dlog("G3D$: destruct shader(%u)", mProgram);
			glDeleteProgram(mProgram);
		}
		
	}
};

Gl3dShader::Gl3dShader()
	: mImpl(new ShaderImpl())
{}

void Gl3dShader::Build(const char * asciitext, ShaderType type)
{
	GLenum e;
	switch (type)
	{
	case ShaderType::VertexShader:
		e = GL_VERTEX_SHADER;
		break;
	case ShaderType::FragmentShader:
		e = GL_FRAGMENT_SHADER;
		break;
	case ShaderType::PixelShader:
		e = 0x435214;
		break;
	default:
		break;
	}
	GLuint shader = glCreateShader(e);
	if (shader < 1)
		throw Gl3dShaderException("error of create v/f shader");

    glShaderSource(shader, 1, &asciitext, NULL);
	glCompileShader(shader);

	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == 0)
	{
		static GLchar sbuffer[1024];
		static GLsizei len;
		glGetShaderInfoLog(shader, 1024, &len, sbuffer);	
		throw Gl3dShaderException(sbuffer);
	}
	glAttachShader(mImpl->mProgram, shader);
	mImpl->mShaders.push_back(shader);
}

void Gl3dShader::Relink()
{
	glLinkProgram(mImpl->mProgram);
	int linkStatus;
	glGetProgramiv(mImpl->mProgram, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == 0)
	{
		static GLchar sbuffer[1024];
		static GLsizei len;
		glGetProgramInfoLog(mImpl->mProgram, 1024, &len, sbuffer);
		throw Gl3dShaderException(sbuffer);
	}
	for (GLuint s : mImpl->mShaders)
		if (s > 0)
			glDeleteShader(s);
	mImpl->mShaders.clear();
}

unsigned int Gl3dShader::GetObject() const
{
	return mImpl->mProgram;
}

int Gl3dShader::UniformLocation(const char * ifName) const
{
	int& a = mImpl->mUniforms[ifName];
	if (a == 0)
		a = glGetUniformLocation(mImpl->mProgram, ifName);
	return a;
}

int Gl3dShader::UniformLocationExt(const char * ifName, int id) const
{
	int& a = mImpl->extBindings[id];
	if (a == 0)
		a = glGetUniformLocation(mImpl->mProgram, ifName);
	return a;
}

int Gl3dShader::MapLocation(const char * ifName, int id) const
{
	int& a = mImpl->mTexturesInner[ifName];
	if (a == 0)
		a = mImpl->mTexturesIter++;
	return a;
}

Gl3dShader::~Gl3dShader()
{
	if (mImpl)
		delete mImpl;
}

const char * Gl3dShaderException::what()  const noexcept
{
	return mErrorString;
}
