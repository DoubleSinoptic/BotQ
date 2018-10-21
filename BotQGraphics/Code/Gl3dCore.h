#pragma once

#include <exception>
#include <cstddef>
#include <initializer_list>

#if defined(_WIN32) || defined(_WIN64)
#define GL3DC_EXPORT __declspec(dllexport)
#else
#define GL3DC_EXPORT
#endif

#ifdef _DEBUG
#include <cstdio>
template<typename... T>
constexpr void __g3dlog(const T&... args)
{
	std::printf(args...);
	std::putchar('\n');
}
#define g3dlog __g3dlog
#define g3dlogD  
#else
#define g3dlog 
#define g3dlogD 
#endif

class GL3DC_EXPORT Gl3dCoreException : public std::exception
{
	const char* mErrorString;
public:
	Gl3dCoreException(const char* errorStirng) : mErrorString(errorStirng) {}
	const char * what() const noexcept
	{
		return mErrorString;
	}
};

template<bool B, class T = void>
struct Gl3dEnableIf {};

template<class T>
struct Gl3dEnableIf<true, T> { typedef T Type; };

template<class T, T v>
struct IntegralConstant {
	static constexpr T Value = v;
	typedef T ValueType;
	typedef IntegralConstant Type;
	constexpr operator ValueType() const noexcept { return Value; }
	constexpr ValueType operator()() const noexcept { return Value; }
};

namespace Gl3dDetail {
	template <typename T> char Test(int T::*);
	struct two { char c[2]; };
	template <typename T> two Test(...);
}

template <class T>
struct IsClass : IntegralConstant<bool, sizeof(Gl3dDetail::Test<T>(0)) == 1>
{};