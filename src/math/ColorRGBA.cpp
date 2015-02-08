/*
 * ColorRGBA.cpp
 *
 *  Created on: Apr 3, 2014
 *      Author: hudo
 */

#include <cmath>
#include <utility>

#include "math/ColorRGBA.h"
#include "math/Math.h"


ColorRGBA operator*(const float lhs, const ColorRGBA& rhs)
{
	return ColorRGBA(rhs.m_data[0]*lhs, rhs.m_data[1]*lhs, rhs.m_data[2]*lhs, rhs.m_data[3]*lhs);
}


ColorRGBA ColorRGBA::operator*(const float rhs) const
{
	return ColorRGBA(m_data[0] * rhs, m_data[1] * rhs, m_data[2] * rhs, m_data[3] * rhs);
}


ColorRGBA ColorRGBA::operator+(const ColorRGBA& rhs) const
{
	return ColorRGBA(m_data[0] + rhs.m_data[0],
				m_data[1] + rhs.m_data[1], m_data[2] + rhs.m_data[2], m_data[3] + rhs.m_data[3]);
}


ColorRGBA ColorRGBA::operator-(const ColorRGBA& rhs) const
{
	return ColorRGBA(m_data[0] - rhs.m_data[0],
				m_data[1] - rhs.m_data[1], m_data[2] - rhs.m_data[2], m_data[3] - rhs.m_data[3]);
}


bool ColorRGBA::operator==(const ColorRGBA& rhs) const
{
	return (m_data[0] == rhs.m_data[0] && m_data[1] == rhs.m_data[1] && m_data[2] == rhs.m_data[2] && m_data[3] == rhs.m_data[3]);
}


ColorRGBA& ColorRGBA::operator=(const ColorRGBA& rhs)
{
	m_data[0] = rhs.m_data[0];
	m_data[1] = rhs.m_data[1];
	m_data[2] = rhs.m_data[2];
	m_data[3] = rhs.m_data[3];

	return *this;
}


ColorRGBA::ColorRGBA(float r, float g, float b, float a)
{
	m_data[0] = r;
	m_data[1] = g;
	m_data[2] = b;
	m_data[3] = a;
}


ColorRGBA::ColorRGBA()
{
	m_data[0] = 0;
	m_data[1] = 0;
	m_data[2] = 0;
	m_data[3] = 1;
}


ColorRGBA::ColorRGBA(const ColorRGBA& v)
{
	m_data[0] = v.m_data[0];
	m_data[1] = v.m_data[1];
	m_data[2] = v.m_data[2];
	m_data[3] = v.m_data[3];
}


ColorRGBA::~ColorRGBA()
{

}
