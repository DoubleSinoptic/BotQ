#include "PathHelper.h"

String Path::FileExt(const String & p)
{
	String s;
	int dx = p.LastIndexOf('.');
	for (int i = dx + 1; i < p.Length(); i++)
		s += p[i];
	return s;
}

String Path::FileName(const String & p)
{
	String s;
	int dx = p.LastIndexOf('/');
	for (int i = dx + 1; i < p.Length(); i++)
		s += p[i];
	return s;
}

String Path::FileDir(const String & p)
{
	String s;
	int dx = p.LastIndexOf('/');
	for (int i = 0; i < dx; i++)
		s += p[i];
	return s;
}

String Path::PathFix(const String & str)
{
	if (str.IsEmpty())
		return "";
	String s;
	String b;
	for (int i = 0; i < str.Length(); i++)
	{
		if (i == 0 && str[i] == '.')
			continue;
		if (str[i] == '\\')
			s += '/';
		else
			s += str[i];
	}
	for (int i = 0; i < s.Length(); i++)
	{
		if (s[i] == '/' && s[i + 1] == '/')
		{
		}
		else
			b += s[i];
	}
	if (b[0] == '/')
		return b;
	return "/" + b;
}
