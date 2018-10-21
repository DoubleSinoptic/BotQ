#include "Resource.h"
#include "GameInstance.h"

String to_lower(const String& str) 
{
	String rez = str;
	for (int i = 0; i < rez.Length(); i++) 
		if (rez[i] >= 'A' && rez[i] <= 'Z')
			rez[i] = 'a' + (rez[i] - 'A');
	return rez;
}

bool compareassigment(const char* path, const char* elem) 
{
	for (; *path == *elem; path++, elem++)
		if (*path == '\0')
			break;
	if (*elem == '/')
		return true;
	return false;
}

Resource::~Resource()
{
	Log("Removed resource: --> %s", *this->mPath);
	if(mAdapter)
		delete mAdapter;
	GameInstance::GetCurrent()->resources.Remove(this);
}

Resource::Resource(const String& path, ResourceAdapter* adapter)
	: mPath(to_lower(path)), mAdapter(adapter)
{
	GameInstance::GetCurrent()->resources.Add(this);
}

DynamicArray<Resource*> Resource::FindAll(const String & folder)
{
	String lw = to_lower(folder);
	DynamicArray<Resource*> r;
	for (Resource* i : GameInstance::GetCurrent()->resources)
		if (compareassigment(*i->GetPath(), *lw))
			r.Add(i);
	return r;
}

Resource * Resource::Find(const String & path, bool throwError)
{
	String lw = to_lower(path);
	DynamicArray<Resource*> r;
	for (Resource* i : GameInstance::GetCurrent()->resources)
		if (i->GetPath() == lw)
			return i;
	if (throwError)
	{
		Error("can not find resource of path: %s", *lw);
	}
		
	else
		return nullptr;
}

void Resource::FreeAllResources()
{
	DynamicArray<Resource*> source;
	source.Initialize(GameInstance::GetCurrent()->resources.Length());
	GameInstance::GetCurrent()->resources.CopyTo(source.GetData());
	for (auto i : source)
		delete i;
}

