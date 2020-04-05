/*
 * Matrix4x4.h
 *
 *  Created on: Apr 2, 2014
 *      Author: hudo
 */

#pragma once

class Matrix4
{
public:
	float m_data[16];

	Matrix4();
	Matrix4(const Matrix4& m);
	Matrix4(Matrix4&& m);

	~Matrix4();

	Matrix4& identity();

	Matrix4& transpose();
	Matrix4 getTranspose() const;

	Matrix4 operator*(const Matrix4& rhs) const;
	Matrix4 operator+(const Matrix4& rhs) const;
	Matrix4 operator-(const Matrix4& rhs) const;
	bool operator==(const Matrix4& rhs) const;
	Matrix4& operator=(const Matrix4& rhs);

};



