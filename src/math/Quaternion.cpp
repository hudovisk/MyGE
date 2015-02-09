/*
 * Quaternion.cpp
 *
 *  Created on: Apr 12, 2014
 *      Author: hudo
 */

#include <cmath>

#include "math/Quaternion.h"

const float PIdiv180 = 0.0174532925199433;

Quaternion::Quaternion(float w, Vec3 axis) :
		m_w(w), m_axis(axis) {

}

Quaternion::Quaternion() {
	m_w = 1;
	m_axis = Vec3(0, 0, 0);
}

Quaternion& Quaternion::inverse() {
	m_axis.inverse();

	return *this;
}

Quaternion Quaternion::getInverse() const {
	return Quaternion(m_w, m_axis.getInverse());
}

Quaternion& Quaternion::normalize() {
	float magnitude = sqrt(m_w*m_w + m_axis.magnitudeSquared());

	m_w /= magnitude;
	m_axis = m_axis * (1/magnitude);

	return *this;
}

Quaternion Quaternion::getNormalized() const {
	Quaternion quaternion(m_w, m_axis);

	float magnitude = sqrt(quaternion.m_w*quaternion.m_w +
												 quaternion.m_axis.magnitudeSquared());

	quaternion.m_w /= magnitude;
	quaternion.m_axis = quaternion.m_axis * (1/magnitude);

	return quaternion;
}

Matrix4 Quaternion::getMatrix4Form() {
	Matrix4 result;
	//just for simplification see Quaternions on wikipedia
	float* mat = result.m_data;
	float w = m_w;
	float x = m_axis.m_data[0];
	float y = m_axis.m_data[1];
	float z = m_axis.m_data[2];

	mat[0] = 1 - 2 * y * y - 2 * z * z;
	mat[1] = 2 * x * y - 2 * z * w;
	mat[2] = 2 * x * z + 2 * y * w;
	mat[3] = 0;
	mat[4] = 2 * x * y + 2 * z * w;
	mat[5] = 1 - 2 * x * x - 2 * z * z;
	mat[6] = 2 * y * z - 2 * x * w;
	mat[7] = 0;
	mat[8] = 2 * x * z - 2 * y * w;
	mat[9] = 2 * y * z + 2 * x * w;
	mat[10] = 1 - 2 * x * x - 2 * y * y;
	mat[11] = 0;
	mat[12] = 0;
	mat[13] = 0;
	mat[14] = 0;
	mat[15] = 1;

	return result;
}

Matrix3 Quaternion::getMatrix3Form()
{
	Matrix3 result;

	float *m = result.m_data;

	float x = m_axis.m_data[0];
	float y = m_axis.m_data[1];
	float z = m_axis.m_data[2];
	float w = m_w;

	m[0] = 1 - 2*y*y - 2*z*z;
	//m[0] = w*w + x*x - y*y - z*z;
	m[1] = 2*x*y - 2*z*w;
	m[2] = 2*x*z + 2*y*w;

	m[3] = 2*x*y + 2*z*w;
	m[4] = 1 - 2*x*x - 2*z*z;
	// m[4] = w*w - x*x + y*y - z*z;
	m[5] = 2*y*z - 2*x*w;

	m[6] = 2*x*z - 2*y*w;
	m[7] = 2*y*z + 2*x*w;
	m[8] = 1 - 2*x*x - 2*y*y;
	// m[8] = w*w - x*x - y*y* + z*z;

	return result;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
	float w = m_w * rhs.m_w - m_axis.dotProduct(rhs.m_axis);
	Vec3 axis = m_w * rhs.m_axis + rhs.m_w * m_axis
			+ m_axis.crossProduct(rhs.m_axis);
	return Quaternion(w, axis);
}

Quaternion Quaternion::operator+(const Quaternion& rhs) const {
	return Quaternion(m_w + rhs.m_w, m_axis + rhs.m_axis);
}

Quaternion Quaternion::operator-(const Quaternion& rhs) const {
	return Quaternion(m_w - rhs.m_w, m_axis - rhs.m_axis);
}

