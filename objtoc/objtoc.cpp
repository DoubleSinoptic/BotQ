#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <filesystem>

namespace fs = std::experimental::filesystem;

const float epsilon = 0.000001f;
struct vec2 { float x; float y; };
struct vec3 { float x; float y; float z; 
	static vec3 cross(const vec3& a, const vec3& b)
	{
		return {
			a.y * b.z - b.y * a.z,
			a.z * b.x - b.z * a.x,
			a.x * b.y - b.x * a.y
		};
	}

	static float dot(const vec3& a, const vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
};
vec3 operator -(const vec3& a, const vec3& b) 
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}
vec3 operator *(const vec3& a, const vec3& b)
{
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}
vec3 operator +(const vec3& a, const vec3& b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}
vec3 operator *(const vec3& a, float b)
{
	return { a.x *b, a.y * b, a.z * b };
}
vec3 operator *(float b, const vec3& a)
{
	return { a.x *b, a.y * b, a.z * b };
}

bool operator ==(const vec3& a, const vec3& b)
{
	return (fabs(a.x - b.x) <= epsilon) &&
		(fabs(a.y - b.y) <= epsilon) &&
		(fabs(a.z - b.z) <= epsilon);
}
bool operator ==(const vec2& a, const vec2& b)
{
	return (fabs(a.x - b.x) <= epsilon) &&
		(fabs(a.y - b.y) <= epsilon);
}




struct poly_mesh 
{
	std::vector<vec3> v;
	std::vector<vec2> t;
	std::vector<vec3> n;
	std::vector<int>  s;
	int founders = 0;
	int add_vertex(
		vec3 va,
		vec2 ta,
		vec3 na)
	{
		for (int i = 0; i < v.size(); i++)
			if (v[i] == va && t[i] == ta && n[i] == na)
			{
				founders++;
				return i;
			}
				
		int i = v.size();
		v.push_back(va);
		t.push_back(ta);
		n.push_back(na);
		return i;
	}

	void add_triangle(
		vec3 va, vec3 vb, vec3 vc,
		vec2 ta, vec2 tb, vec2 tc,
		vec3 na, vec3 nb, vec3 nc,
		bool useOrderFixAlgo = false
	) 
	{
		if (useOrderFixAlgo)
		{
			vec3 center = (va + vb + vc) * (1.0f / 3.0f);
			vec3 du = vec3::cross(va - vb, va - vc);
			vec3 dd = vec3{ 0.0f, 0.0f , 0.0f } -center;
			if (vec3::dot(du, dd) <= 0.5)
			{
				s.push_back(add_vertex(vc, tc, nc));
				s.push_back(add_vertex(vb, tb, nb));
				s.push_back(add_vertex(va, ta, na));
			}
			else
			{
				s.push_back(add_vertex(va, ta, na));
				s.push_back(add_vertex(vb, tb, nb));
				s.push_back(add_vertex(vc, tc, nc));
			}
		}
		else 
		{
			s.push_back(add_vertex(va, ta, na));
			s.push_back(add_vertex(vb, tb, nb));
			s.push_back(add_vertex(vc, tc, nc));
		}
		
			
	}
};
poly_mesh loadModel(std::istream& f)
{
	poly_mesh r;
	std::vector<vec3> vr;
	std::vector<vec2> tr;
	std::vector<vec3> nr;

	std::string a, b, c;
	std::string s;
	while (f >> s)
	{
		if (s == "v")
		{
			float x, y, z;
			f >> x >> y >> z;
			vr.push_back({x, y, z});
		
		}
		if (s == "vn")
		{
			float x, y, z;
			f >> x >> y >> z;
			nr.push_back({ x, y, z });
		}

		if (s == "vt")
		{
			float x, y;
			f >> x >> y;
			tr.push_back({ x, y });
		}

		if (s == "f")
		{
			a.clear();
			b.clear();
			c.clear();
			f >> a >> b >> c;
			int v, t, n;
			int v1, t1, n1;
			int v2, t2, n2;
			sscanf(a.data(), "%d/%d/%d", &v, &t, &n);
			sscanf(b.data(), "%d/%d/%d", &v1, &t1, &n1);
			sscanf(c.data(), "%d/%d/%d", &v2, &t2, &n2);
			v--, t--, n--;
			v1--, t1--, n1--;
			v2--, t2--, n2--;

			r.add_triangle(vr[v], vr[v1], vr[v2],
				tr[t], tr[t1], tr[t2],
				nr[n], nr[n1], nr[n2]);
		}
	}
	return r;
}


const char* sample =
"#ifndef __%s_H__\n"
"#define __%s_H__\n"
"\n"
"namespace model_%s {\n"
"	static const float vertexes[] = %s;\n"
"	static const float normals[] = %s;\n"
"	static const float texcoords[] = %s;\n"
"	static const int   indeces[] = %s;\n"
"	static int		   vertexesCount = %d;\n"
"	static int		   indecesCount = %d;\n"
"}\n"
"#endif ";

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) 
	{
		fs::path p = argv[i];
		std::ifstream fs(p.string(), std::ios::binary);
		poly_mesh m = loadModel(fs);
		std::string fn = p.replace_extension("").filename().string();

		std::ofstream o("./" + fn + ".h", std::ios::binary);
		
		std::string v = "{ ";
		std::string n = "{ ";
		std::string t = "{ ";
		std::string in = "{ ";
		for (int i = 0; i < m.v.size(); i++)
			v += std::to_string(m.v[i].x) + "f, " + std::to_string(m.v[i].y) + "f, " + std::to_string(m.v[i].z) + (i + 1 == m.v.size() ? "f }" : "f, ");

		for (int i = 0; i < m.t.size(); i++)
			t += std::to_string(m.t[i].x) + "f, " + std::to_string(m.t[i].y) + (i + 1 == m.t.size() ? "f }" : "f, ");

		for (int i = 0; i < m.n.size(); i++)
			n += std::to_string(m.n[i].x) + "f, " + std::to_string(m.n[i].y) + "f, " + std::to_string(m.n[i].z) + (i + 1 == m.n.size() ? "f }" : "f, ");

		for (int i = 0; i < m.s.size(); i++)
			in += std::to_string(m.s[i]) + (i + 1 == m.s.size() ? " }" : ", ");


		const char* filename = fn.c_str();
		std::string r(
			std::snprintf(nullptr, 0,  sample, filename, filename, filename, v.c_str(), n.c_str(), t.c_str(), in.c_str(), m.v.size(), m.s.size()), '\A');
		std::snprintf((char*)r.data(), r.length(), sample, filename, filename, filename, v.c_str(), n.c_str(), t.c_str(), in.c_str(), m.v.size(), m.s.size());
	
		r[r.size() - 1] = '\n';
		o << r;

	}
    std::cout << "Hello World!\n"; 
	return 0;
}


