/*
 * Quaternion.h
 *
 *  Created on: Apr 11, 2014
 *      Author: hudo
 */
#pragma once

#include "math/Vec3.h"
#include "math/Matrix4.h"
#include "math/Matrix3.h"

class Quaternion
{
public:
	Quaternion(float w, Vec3 axis);
	Quaternion();

	Quaternion& inverse();
	Quaternion getInverse() const;

	Quaternion& normalize();
	Quaternion getNormalized() const;

	Matrix4 getMatrix4Form();
	Matrix3 getMatrix3Form();

	Quaternion operator*(const Quaternion& rhs) const;
	Quaternion operator+(const Quaternion& rhs) const;
	Quaternion operator-(const Quaternion& rhs) const;

	float m_w;
	Vec3 m_axis;
};
