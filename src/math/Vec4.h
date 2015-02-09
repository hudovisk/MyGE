/*
 * Vec4.h
 *
 *  Created on: Apr 2, 2014
 *      Author: hudo
 */

#pragma once

#include "Matrix4.h"
#include "Vec3.h"

class Vec4
{
public:
	float m_data[4];

	Vec4 operator*(const Matrix4& rhs) const;
	Vec4 operator*(const float rhs) const;
	Vec4 operator+(const Vec4& rhs) const;
	Vec4 operator-(const Vec4& rhs) const;
	bool operator==(const Vec4& rhs) const;
	Vec4& operator=(const Vec4& rhs);

	float dotProduct(const Vec4& rhs) const;
	float dotProduct3(const Vec4& rhs) const;

	Vec4 crossProduct(const Vec4& rhs) const;

	float magnitude() const;
	float magnitude3() const;

	Vec4& normalize();
	Vec4& normalize3();

	Vec4(float x, float y, float z, float w);
	Vec4();

	Vec4(const Vec4& v);
	Vec4(const Vec3& v3, float w);

	Vec4(Vec4&& v);
	~Vec4();
};

Vec4 operator*(const float lhs, const Vec4& rhs);





