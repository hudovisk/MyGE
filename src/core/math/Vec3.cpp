/*
 * Vec3.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: hudo
 */

#include <cmath>
#include <utility>

#include "Vec3.h"

Vec3 operator*(const float lhs, const Vec3& rhs)
{
	return Vec3(rhs.m_data[0]*lhs, rhs.m_data[1]*lhs, rhs.m_data[2]*lhs);
}

Vec3 Vec3::operator*(const float rhs) const
{
	return Vec3(m_data[0] * rhs, m_data[1] * rhs, m_data[2] * rhs);
}

Vec3 Vec3::operator+(const Vec3& rhs) const
{
	return Vec3(m_data[0] + rhs.m_data[0],
				m_data[1] + rhs.m_data[1],
				m_data[2] + rhs.m_data[2]);
}

Vec3 Vec3::operator-(const Vec3& rhs) const
{
	return Vec3(m_data[0] - rhs.m_data[0],
				m_data[1] - rhs.m_data[1],
				m_data[2] - rhs.m_data[2]);
}

bool Vec3::operator==(const Vec3& rhs) const
{
	return (m_data[0] == rhs.m_data[0] && m_data[1] == rhs.m_data[1] && m_data[2] == rhs.m_data[2]);
}

Vec3& Vec3::operator=(const Vec3& rhs)
{
	m_data[0] = rhs.m_data[0];
	m_data[1] = rhs.m_data[1];
	m_data[2] = rhs.m_data[2];

	return *this;
}

float Vec3::dotProduct(const Vec3& rhs) const
{
	return 	m_data[0] * rhs.m_data[0] +
			m_data[1] * rhs.m_data[1] +
			m_data[2] * rhs.m_data[2];
}

Vec3 Vec3::crossProduct(const Vec3& rhs) const
{
	return Vec3(m_data[1]*rhs.m_data[2] - m_data[2]*rhs.m_data[1],
				m_data[2]*rhs.m_data[0] - m_data[0]*rhs.m_data[2],
				m_data[0]*rhs.m_data[1] - m_data[1]*rhs.m_data[0]);
}

float Vec3::magnitude() const
{
	return sqrt(m_data[0]*m_data[0] +
				m_data[1]*m_data[1] +
				m_data[2]*m_data[2]);
}

float Vec3::magnitudeSquared() const
{
	return 	m_data[0]*m_data[0] +
			m_data[1]*m_data[1] +
			m_data[2]*m_data[2];
}

Vec3& Vec3::normalize()
{
	float mag = magnitude();
	m_data[0] /= mag;
	m_data[1] /= mag;
	m_data[2] /= mag;

	return *this;
}

Vec3& Vec3::inverse()
{
	m_data[0] = -m_data[0];
	m_data[1] = -m_data[1];
	m_data[2] = -m_data[2];

	return *this;
}

Vec3 Vec3::getInverse() const
{
	return Vec3(-m_data[0],-m_data[1],-m_data[2]);
}

Vec3::Vec3(float x, float y, float z)
{
	m_data[0] = x;
	m_data[1] = y;
	m_data[2] = z;
}

Vec3::Vec3()
{
	m_data[0] = 0;
	m_data[1] = 0;
	m_data[2] = 0;
}

Vec3::Vec3(const Vec3& v)
{
	m_data[0] = v.m_data[0];
	m_data[1] = v.m_data[1];
	m_data[2] = v.m_data[2];
}

Vec3::Vec3(Vec3&& v)
{
	std::swap(m_data,v.m_data);
}

Vec3::~Vec3()
{

}
