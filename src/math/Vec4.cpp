/*
 * Vec4.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: hudo
 */

#include "math/Vec4.h"
#include <cmath>
#include <utility>

Vec4 operator*(const float lhs, const Vec4& rhs)
{
	return Vec4(rhs.m_data[0]*lhs,rhs.m_data[1]*lhs, rhs.m_data[2]*lhs, rhs.m_data[3]*lhs);
}

Vec4::Vec4()
{
	for(int i = 0; i < 4; i++)
	{
		m_data[i] = 0;
	}
}

Vec4::Vec4(const Vec4& v)
{
	m_data[0] = v.m_data[0];
	m_data[1] = v.m_data[1];
	m_data[2] = v.m_data[2];
	m_data[3] = v.m_data[3];
}

Vec4::Vec4(Vec4&& v)
{
	std::swap(m_data,v.m_data);
}


Vec4::Vec4(const Vec3& v3, float w)
{
	m_data[0] = v3.m_data[0];
	m_data[1] = v3.m_data[1];
	m_data[2] = v3.m_data[2];
	m_data[3] = w;
}

Vec4::~Vec4()
{

}

Vec4::Vec4(float x, float y, float z, float w)
{
	m_data[0] = x;
	m_data[1] = y;
	m_data[2] = z;
	m_data[3] = w;
}

Vec4 Vec4::operator*(const Matrix4& rhs) const
{
	Vec4 result;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			result.m_data[i] += m_data[j] * rhs.m_data[i + j*4];
		}
	}
	return result;
}

Vec4 Vec4::operator*(const float rhs) const
{
	return Vec4(m_data[0] * rhs, m_data[1] * rhs, m_data[2] * rhs, m_data[3] * rhs);
}

Vec4 Vec4::operator+(const Vec4& rhs) const
{
	return Vec4(m_data[0] + rhs.m_data[0], m_data[1] + rhs.m_data[1],
				m_data[2] + rhs.m_data[2], m_data[3] + rhs.m_data[3]);
}

Vec4 Vec4::operator-(const Vec4& rhs) const
{
	return Vec4(m_data[0] - rhs.m_data[0], m_data[1] - rhs.m_data[1],
				m_data[2] - rhs.m_data[2], m_data[3] - rhs.m_data[3]);
}

bool Vec4::operator==(const Vec4& rhs) const
{
	return (m_data[0] == rhs.m_data[0] &&
			m_data[1] == rhs.m_data[1] &&
			m_data[2] == rhs.m_data[2] &&
			m_data[3] == rhs.m_data[3]);
}

Vec4& Vec4::operator=(const Vec4& rhs)
{
	m_data[0] = rhs.m_data[0];
	m_data[1] = rhs.m_data[1];
	m_data[2] = rhs.m_data[2];
	m_data[3] = rhs.m_data[3];

	return *this;
}

float Vec4::dotProduct(const Vec4& rhs) const
{
	float result = 0;
	for(int i = 0; i < 4; i++)
	{
		result += m_data[i] * rhs.m_data[i];
	}
	return result;
}

float Vec4::dotProduct3(const Vec4& rhs) const
{
	float result = 0;
	for(int i = 0; i < 3; i++)
	{
		result += m_data[i] * rhs.m_data[i];
	}
	return result;
}

Vec4 Vec4::crossProduct(const Vec4& rhs) const
{
	return Vec4(m_data[1]*rhs.m_data[2] - m_data[2]*rhs.m_data[1],
				m_data[2]*rhs.m_data[0] - m_data[0]*rhs.m_data[2],
				m_data[0]*rhs.m_data[1] - m_data[1]*rhs.m_data[0],
				0);
}

float Vec4::magnitude() const
{
	return sqrt(m_data[0]*m_data[0] +
				m_data[1]*m_data[1] +
				m_data[2]*m_data[2] +
				m_data[3]*m_data[3]);
}

float Vec4::magnitude3() const
{
	return sqrt(m_data[0]*m_data[0] +
				m_data[1]*m_data[1] +
				m_data[2]*m_data[2]);
}

Vec4& Vec4::normalize()
{
	float mag = magnitude();
	m_data[0] /= mag;
	m_data[1] /= mag;
	m_data[2] /= mag;
	m_data[3] /= mag;

	return *this;
}

Vec4& Vec4::normalize3()
{
	float mag = magnitude3();
	m_data[0] /= mag;
	m_data[1] /= mag;
	m_data[2] /= mag;
	m_data[3] = 0;
	return *this;
}
