#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define SGE_EXPORT __declspec(dllexport)
#else
#define SGE_EXPORT
#endif

//#define USE_INSTANCING

#define FORCEINLINE inline