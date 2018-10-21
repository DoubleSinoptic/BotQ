#pragma once
#ifndef _ResourceLoader_H_
#define _ResourceLoader_H_

#include "Core.h"

class SGE_EXPORT ResourceLoader
{
public:
	static void LoadResourcesPk(const String& path);
	static void LoadResourcesPk2(const String& path);
	static void LoadStaticAssets(const String& apth);
};

#endif