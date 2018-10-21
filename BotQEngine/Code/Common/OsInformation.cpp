#include "OsInformation.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

const char* SystemInfo::OsVersionString()
{
	return
#ifdef _WIN64
		"Windows x64"
#elif _WIN32
		"Windows x32"
#elif __FreeBSD__
		"FreeBSD"
#elif __ANDROID__
		"Android"
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
		"IOS"
#elif TARGET_OS_IPHONE
		"IOS"
#elif TARGET_OS_MAC
		"Mac OS"
#else
		"Unknown Apple platform"
#endif
#elif __linux
		"Linux"
#elif __unix // all unices not caught above
		"Unix"
#elif __posix
		"Posix"
#endif
		;
}

int SystemInfo::BitsForWord()
{
	return sizeof(size_t) * 8;
}

bool SystemInfo::IsLitleEndian()
{
	unsigned int value = 1;
	return (*((unsigned char*)&value) == 1);
}
