/*
 * Matrix3.h
 *
 *  Created on: 03/06/2014
 *      Author: hudo
 */

#pragma once

class Matrix3
{
public:
	float m_data[9];

	Matrix3();
	Matrix3(const Matrix3& m);
	Matrix3(Matrix3&& m);

	~Matrix3();

	Matrix3& identity();

	Matrix3& transpose();
	Matrix3 getTranspose() const;

	Matrix3 operator*(const Matrix3& rhs) const;
	Matrix3 operator+(const Matrix3& rhs) const;
	Matrix3 operator-(const Matrix3& rhs) const;
	bool operator==(const Matrix3& rhs) const;
	Matrix3& operator=(const Matrix3& rhs);
};
