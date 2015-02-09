#include <utility>
#include <cstring>

#include "math/Matrix3.h"

Matrix3::Matrix3()
{
	identity();
}

Matrix3::Matrix3(const Matrix3& m)
{
	memcpy(m_data,m.m_data,9*sizeof(float));
}

Matrix3::Matrix3(Matrix3&& m)
{
	std::swap(m_data,m.m_data);
}

Matrix3::~Matrix3()
{

}

Matrix3& Matrix3::identity()
{
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			m_data[i*3 + j] = 0;
		}
		m_data[i*3 + i] = 1;
	}

	return *this;
}

Matrix3& Matrix3::transpose()
{
	Matrix3 aux(*this);
	for(int i=0; i<3; i++)
	{
		for(int j=i; j<3; j++)
		{
			m_data[i*3 + j] = aux.m_data[j*3 + i];
		}
	}

	return *this;
}
Matrix3 Matrix3::getTranspose() const
{
	Matrix3 aux(*this);
	for(int i=0; i<3; i++)
	{
		for(int j=i; j<3; j++)
		{
			aux.m_data[i*3 + j] = m_data[j*3 + i];
		}
	}

	return aux;
}

Matrix3 Matrix3::operator*(const Matrix3& rhs) const
{
	Matrix3 result;
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			float soma = 0;
			for(int k=0; k<3; k++)
			{
				soma += m_data[i*3 + k] * rhs.m_data[k*3 +j];
			}
			result.m_data[i*3 +j] = soma;
		}
	}

	return result;
}

Matrix3 Matrix3::operator+(const Matrix3& rhs) const
{
	Matrix3 result;
	for(int i=0; i<9; i++)
	{
		result.m_data[i] = m_data[i] + rhs.m_data[i];
	}

	return result;
}

Matrix3 Matrix3::operator-(const Matrix3& rhs) const
{
	Matrix3 result;
	for(int i=0; i<9; i++)
	{
		result.m_data[i] = m_data[i] - rhs.m_data[i];
	}

	return result;
}

bool Matrix3::operator==(const Matrix3& rhs) const
{
	for(int i=0; i<9; i++)
	{
		if(m_data[i] != rhs.m_data[i])
			return false;
	}

	return true;
}

Matrix3& Matrix3::operator=(const Matrix3& rhs)
{
	memcpy(m_data, rhs.m_data,9*sizeof(float));

	return *this;
}



