
#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include "render/Texture.h"
#include "math/Vec3.h"

struct Vertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;

	Vertex()
	{
		x = y = z = 0;
		nx = ny = nz = 0;
		u = v = 0;
	}

	Vertex(Vec3 pos, Vec3 normal, Vec3 texture)
	{
		x = pos.m_data[0]; 			y = pos.m_data[1]; 			z = pos.m_data[2];
		nx = normal.m_data[0]; 	ny = normal.m_data[1]; 	nz = normal.m_data[2];
		u = texture.m_data[0]; 	v = texture.m_data[1];
	}

};

class Geometric
{
public:
	Geometric() 
		: m_texture(nullptr), m_vao(0), m_vbo(0),
		m_vboIndices(0), m_numIndices(0), m_isInitialised(false)
	{

	}

	Texture* m_texture;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_vboIndices;	
	unsigned int m_numIndices;

	bool m_isInitialised;
};

#endif //GEOMETRIC_H