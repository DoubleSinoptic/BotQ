#include "TypedObject.h"

TypedObject::~TypedObject() {}

void TypedObject::CloneTo(TypedObject * obj) const
{
	
}

TypedObject::TypedObject() 
{}

ReferencableProxy * TypedObject::BeginProxy()
{
	return _prefproxy;
}

int TypedObject::GetProxyCount()
{
	int rz = 0;
	ReferencableProxy* l = _prefproxy;
	while (l)
	{
		rz++;
		l = l->last;
	}
	return rz;
}

ReferencableProxy * TypedObject::FindPropetry(const char * name)
{
	ReferencableProxy* l = _prefproxy;
	while (l)
	{
		if (strcmp(l->name, name) == 0)
			return l;
		l = l->last;
	}
	return nullptr;
}
