
#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include "render/Texture.h"
#include "math/Vec3.h"

struct Vertex
{
	float x, y, z;
	float nx, ny, nz;
	float tx, ty, tz;
	float u, v;

	Vertex()
	{
		x = y = z = 0;
		nx = ny = nz = 0;
		tx = ty = tz = 0;
		u = v = 0;
	}

	Vertex(Vec3 pos, Vec3 normal, Vec3 tangent, Vec3 texture)
	{
		x = pos.m_data[0]; 			y = pos.m_data[1]; 			z = pos.m_data[2];
		nx = normal.m_data[0]; 		ny = normal.m_data[1]; 		nz = normal.m_data[2];
		tx = tangent.m_data[0]; 	ty = tangent.m_data[1]; 	tz = tangent.m_data[2];
		u = texture.m_data[0]; 		v = texture.m_data[1];
	}

};

class Material
{
public:
	Material() 
		: m_diffuseTexture(nullptr), m_normalsTexture(nullptr)
	{ }
	~Material() { }
	
	Texture* m_diffuseTexture;
	Texture* m_normalsTexture;
	Vec3 m_diffColor;
	Vec3 m_ambColor;
	Vec3 m_specColor;
	float m_shininess;
};

class Geometric
{
public:
	Geometric() 
		: m_vao(0), m_vbo(0),
		m_vboIndices(0), m_numIndices(0), m_isInitialised(false)
	{

	}

	Material m_material;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_vboIndices;	
	unsigned int m_numIndices;

	bool m_isInitialised;
};

#endif //GEOMETRIC_H