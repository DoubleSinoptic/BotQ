#include <string>
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const unsigned hedaer = 0xFF13fAFF11;

template<typename T>
void stream_write(std::ofstream& str, const T& value)
{
	str.write((const char*)&value, sizeof(T));
}

int main(int argc, char** argv)
{

	for (int i = 1; i < argc; i++)
	{

		std::ofstream of(argv[i], std::ios::binary);
		
		of.read();

	}

    return 0;
}

