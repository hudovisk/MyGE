/*
 * Vec2.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: hudo
 */

#include <cmath>
#include <utility>

#include "math/Vec2.h"
#include "math/Math.h"


Vec2 operator*(const float lhs, const Vec2& rhs)
{
	return Vec2(rhs.m_data[0]*lhs, rhs.m_data[1]*lhs);
}


Vec2 Vec2::operator*(const float rhs) const
{
	return Vec2(m_data[0] * rhs, m_data[1] * rhs);
}


Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(m_data[0] + rhs.m_data[0],
				m_data[1] + rhs.m_data[1]);
}


Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(m_data[0] - rhs.m_data[0],
				m_data[1] - rhs.m_data[1]);
}


Vec2 Vec2::operator-() const
{
	return Vec2(-m_data[0], -m_data[1]);
}


bool Vec2::operator==(const Vec2& rhs) const
{
	return (m_data[0] == rhs.m_data[0] && m_data[1] == rhs.m_data[1]);
}


Vec2& Vec2::operator=(const Vec2& rhs)
{
	m_data[0] = rhs.m_data[0];
	m_data[1] = rhs.m_data[1];

	return *this;
}


float Vec2::dotProduct(const Vec2& rhs) const
{
	return 	m_data[0] * rhs.m_data[0] +
			m_data[1] * rhs.m_data[1];
}


float Vec2::magnitude() const
{
	return sqrt(m_data[0]*m_data[0] +
				m_data[1]*m_data[1]);
}


float Vec2::magnitudeSquared() const
{
	return 	m_data[0]*m_data[0] +
			m_data[1]*m_data[1];
}


Vec2& Vec2::normalize()
{
	float mag = magnitude();
	if(Math::EPSILON > mag && -Math::EPSILON < mag) // do not divide by zero
		return *this;

	m_data[0] /= mag;
	m_data[1] /= mag;
	m_data[2] /= mag;

	return *this;
}


Vec2& Vec2::inverse()
{
	m_data[0] = -m_data[0];
	m_data[1] = -m_data[1];
	m_data[2] = -m_data[2];

	return *this;
}


Vec2::Vec2(float x, float y)
{
	m_data[0] = x;
	m_data[1] = y;
}


Vec2::Vec2()
{
	m_data[0] = 0;
	m_data[1] = 0;
}


Vec2::Vec2(const Vec2& v)
{
	m_data[0] = v.m_data[0];
	m_data[1] = v.m_data[1];
}


Vec2::~Vec2()
{

}
