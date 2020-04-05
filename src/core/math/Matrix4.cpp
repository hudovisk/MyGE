/*
 * Matrix4.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: hudo
 */

#include <utility>
#include <cstring>

#include "Matrix4.h"

Matrix4::Matrix4()
{
	identity();
}

Matrix4::Matrix4(const Matrix4& m)
{
	memcpy(m_data,m.m_data,16*sizeof(float));
}

Matrix4::Matrix4(Matrix4&& m)
{
	std::swap(m_data,m.m_data);
}

Matrix4::~Matrix4()
{

}

Matrix4& Matrix4::identity()
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			m_data[i*4 + j] = 0;
		}
		m_data[i*4 + i] = 1;
	}

	return *this;
}

Matrix4& Matrix4::transpose()
{
	for(int i=0; i<4; i++)
	{
		for(int j=i; j<4; j++)
		{
			std::swap(m_data[i*4 + j],m_data[j*4 + i]);
		}
	}

	return *this;
}
Matrix4 Matrix4::getTranspose() const
{
	Matrix4 result(*this);

	return result.transpose();
}

Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
	Matrix4 result;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			float soma = 0;
			for(int k=0; k<4; k++)
			{
				soma += m_data[i*4 + k] * rhs.m_data[k*4 +j];
			}
			result.m_data[i*4 +j] = soma;
		}
	}

	return result;
}

Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	Matrix4 result;
	for(int i=0; i<16; i++)
	{
		result.m_data[i] = m_data[i] + rhs.m_data[i];
	}

	return result;
}

Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	Matrix4 result;
	for(int i=0; i<16; i++)
	{
		result.m_data[i] = m_data[i] - rhs.m_data[i];
	}

	return result;
}

bool Matrix4::operator==(const Matrix4& rhs) const
{
	for(int i=0; i<16; i++)
	{
		if(m_data[i] != rhs.m_data[i])
			return false;
	}

	return true;
}

Matrix4& Matrix4::operator=(const Matrix4& rhs)
{
	memcpy(m_data, rhs.m_data,16*sizeof(float));

	return *this;
}



