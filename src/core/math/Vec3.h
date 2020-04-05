/*
 * Vec3.h
 *
 *  Created on: Apr 3, 2014
 *      Author: hudo
 */

#pragma once

class Vec3
{
public:
	float m_data[3];

	Vec3 operator*(const float rhs) const;
	Vec3 operator+(const Vec3& rhs) const;
	Vec3 operator-(const Vec3& rhs) const;
	bool operator==(const Vec3& rhs) const;
	Vec3& operator=(const Vec3& rhs);

	float dotProduct(const Vec3& rhs) const;
	Vec3 crossProduct(const Vec3& rhs) const;

	float magnitude() const;
	float magnitudeSquared() const;

	Vec3& normalize();

	Vec3& inverse();
	Vec3 getInverse() const;

	Vec3(float x, float y, float z);
	Vec3();
	Vec3(const Vec3& v);
	Vec3(Vec3&& v);
	~Vec3();
};

Vec3 operator*(const float lhs, const Vec3& rhs);
