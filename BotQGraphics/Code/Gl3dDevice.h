#pragma once

#include "Gl3dCore.h"

enum class ConfigurateFlags
{
	None,
	HinInstance,
	D3dBase,
	D3dBaseIfHinInstance
};

typedef void* (*Gl3dDeviceLinker)(const char *name);

enum 
{
	GL3D_PROFILE_ACTIVETE_CALLS = 0,
	GL3D_PROFILE_UNIFORM_CALLS = 1,
	GL3D_PROFILE_CLEAR_CALLS = 2
};

#define GL3D_COUNTER (Gl3dDevice::GetProfilerData())
#define GL3D_COUNTER_INCREMENT(i) (Gl3dDevice::GetProfilerData()[(i)]++)
class GL3DC_EXPORT Gl3dDevice
{
public:
	static void GetMemory(long long& aviable, long long& total);
	static void AlphaTest(bool value);
    static void DepthTest(bool value);
    static void CullTest(bool value);
	static void SrTest(bool value);
	static void MultiSampleTest(bool value);
	static void LinkAddresses(Gl3dDeviceLinker lnk);
	static void Clear();
	static void Viewport(int i, int itox);
	static void Configurate();
	static void CullBack();
	static void CullFront();
	/*static void DepthMask(bool val);
	static void InsertDepthMod(bool isLess);*/
	static void SkipErrors();
	static void ValidateContext();
	static void ExecCommands();
	static int* GetProfilerData();
	static void ResetProfiler();
	static void SwapToRGB8BPP(void* pixelBuffer, int w, int h);
    static void FramebufferPush();
    static void FramebufferPop();
    static void FramebufferRestore();
};