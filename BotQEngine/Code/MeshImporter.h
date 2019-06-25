#pragma once

#include "Core.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "ModelConstructor.h"

class SGE_EXPORT MeshImporter
{
public:
	static Ref<ModelConstructor> Import(String file/*, const String& txtureAdditionalPath = ""*/);
	static Ref<ModelConstructor> ImportRW(String path, const char* data, size_t len/*, const String& txtureAdditionalPath = ""*/);
};

