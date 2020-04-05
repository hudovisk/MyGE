
#include <cmath>
#include "Transform.h"

const float PIdiv180 = 0.0174532925199433;

float getDeterminant3(float m1, float m2, float m3,
				  	  float m4, float m5, float m6,
				  	  float m7, float m8, float m9)
{
	return (m1 * m5 * m9 + m2 * m6 * m7 + m3 * m4 * m8)
			- (m3 * m5 * m7 + m2 * m4 * m9 + m1 * m6 * m8);
}

float getDeterminant3x3(float *m)
{
	return (m[0] * m[4] * m[8] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7])
			- (m[2] * m[4] * m[6] + m[1] * m[3] * m[8] + m[0] * m[5] * m[7]);
}

float aux[9];
float getCofator(int r, int c, float *m)
{
	int idx = 0;
	for(int i = 0; i < 4; i++)
	{
		if(i == r)
			continue;
		for(int j = 0; j < 4; j++)
		{
			if(j == c)
				continue;
			aux[idx] = m[i*4 + j];
			idx++;
		}
	}

	return (r+c+2)%2 ? -getDeterminant3x3(aux) : getDeterminant3x3(aux);
}

float getDeterminant4x4(float *m)
{
	float cofator11 = getDeterminant3(m[5],  m[6],  m[7],
									  m[9],  m[10], m[11],
									  m[13], m[14], m[15]);


	float cofator12 = getDeterminant3(m[4],  m[6],  m[7],
									   m[8],  m[10], m[11],
									   m[12], m[14], m[15]);

	float cofator13 = getDeterminant3(m[4],  m[5],  m[7],
									  m[8],  m[9],  m[11],
									  m[12], m[13], m[15]);

	float cofator14 = getDeterminant3(m[4],  m[5],  m[6],
									   m[8],  m[9],  m[10],
									   m[12], m[13], m[14]);

	return m[0]*cofator11 - m[1]*cofator12 + m[2]*cofator13 - m[3]*cofator14;
}

Transform::Transform()
    : m_rotation(Quaternion(1,Vec3(0,0,0))), m_translation(Vec3(0,0,0)), m_scale(Vec3(1,1,1)),
        m_matrix(Matrix4()), m_inverseMatrix(Matrix4()), m_dirty(true),
        m_inverseDirty(true)
{

}

Transform::~Transform()
{

}

Quaternion Transform::getRotation()
{
	return m_rotation;
}

void Transform::setRotation(Quaternion quaternion)
{
	m_rotation = quaternion;

	m_dirty = true;
	m_inverseDirty = true;
}

void Transform::setRotationInRadian(float angle, Vec3 axis)
{
	// if(axis.magnitudeSquared() < 0.9999 || axis.magnitudeSquared() > 1.0001)
		axis.normalize();

	float halfAngle = angle * 0.5;
	axis = axis * sin(halfAngle);
	Quaternion quaternion(cos(halfAngle), axis);
	quaternion.normalize();

	m_rotation = quaternion;

  m_dirty = true;
  m_inverseDirty = true;
}

void Transform::setRotationInDegree(float angle, Vec3 axis)
{
	setRotationInRadian(angle * PIdiv180, axis);
}

void Transform::rotateInRadian(float angle, Vec3 axis)
{
	// if(axis.magnitudeSquared() < 0.9999 || axis.magnitudeSquared() > 1.0001)
		axis.normalize();

	float halfAngle = angle * 0.5;
	axis = axis * sin(halfAngle);
	Quaternion quaternion(cos(halfAngle), axis);
	quaternion.normalize();

  m_rotation = m_rotation * quaternion;
  //m_rotation = quaternion * m_rotation;

  m_dirty = true;
  m_inverseDirty = true;
}

void Transform::rotateInDegree(float angle, Vec3 axis)
{
	rotateInRadian(angle * PIdiv180, axis);
}

Vec3 Transform::transform(const Vec4& v)
{
	getMatrix();

	const Vec4 aux = v * m_matrix;
	return Vec3(aux.m_data[0] /*/ aux.m_data[3]*/,
							aux.m_data[1] /*/ aux.m_data[3]*/,
							aux.m_data[2] /*/ aux.m_data[3]*/);
}

void Transform::translate(Vec3 pos)
{
  m_translation = pos;

  m_dirty = true;
  m_inverseDirty = true;
}

void Transform::scale(Vec3 scale)
{
  m_scale = scale;

  m_dirty = true;
}

//Matrix4 Transform::getMatrix()
//{
//    if(!m_dirty)
//    	return m_matrix;
//
//    Matrix4 rotationMatrix = m_rotation.getMatrix4Form();
//    Matrix4 translationMatrix;
//    Matrix4 scaleMatrix;
//
//    translationMatrix.m_data[3] = m_translation.m_data[0];
//    translationMatrix.m_data[7] = m_translation.m_data[1];
//    translationMatrix.m_data[11] = m_translation.m_data[2];
//
//    scaleMatrix.m_data[0] = m_scale.m_data[0];
//    scaleMatrix.m_data[5] = m_scale.m_data[1];
//    scaleMatrix.m_data[10] = m_scale.m_data[2];
//
//    //m_matrix = rotationMatrix * translationMatrix * scaleMatrix;
//    m_matrix = translationMatrix * scaleMatrix * rotationMatrix;
//    //m_matrix = rotationMatrix * translationMatrix;
//
////    m_matrix.m_data[0] *= m_scale.m_data[0];
////    m_matrix.m_data[5] *= m_scale.m_data[1];
////    m_matrix.m_data[10] *= m_scale.m_data[2];
//
////    m_matrix.m_data[3] = m_translation.m_data[0];
////	m_matrix.m_data[7] = m_translation.m_data[1];
////	m_matrix.m_data[11] = m_translation.m_data[2];
//
//    m_dirty = false;
//
//    return m_matrix;
//}

Matrix4& Transform::getMatrix()
{
    if(!m_dirty)
    	return m_matrix;

    m_matrix.identity();

    Matrix3 rotationMatrix = m_rotation.getMatrix3Form();

    //rotation and scale part
    m_matrix.m_data[0] = rotationMatrix.m_data[0] * m_scale.m_data[0];
    m_matrix.m_data[1] = rotationMatrix.m_data[1];
    m_matrix.m_data[2] = rotationMatrix.m_data[2];

    m_matrix.m_data[4] = rotationMatrix.m_data[3];
    m_matrix.m_data[5] = rotationMatrix.m_data[4] * m_scale.m_data[1];
    m_matrix.m_data[6] = rotationMatrix.m_data[5];

    m_matrix.m_data[8] = rotationMatrix.m_data[6];
    m_matrix.m_data[9] = rotationMatrix.m_data[7];
    m_matrix.m_data[10] = rotationMatrix.m_data[8] * m_scale.m_data[2];

    //translate part
    m_matrix.m_data[12] = m_translation.m_data[0];
    m_matrix.m_data[13] = m_translation.m_data[1];
    m_matrix.m_data[14] = m_translation.m_data[2];

    m_dirty = false;

    return m_matrix;
}

Matrix4& Transform::getInverseMatrix()
{
	if(!(m_dirty || m_inverseDirty))
		return m_inverseMatrix;

	getMatrix();

	m_inverseDirty = false;

	float *m = m_matrix.m_data;

	float det = getDeterminant4x4(m);
	if(fabs(det) < 0.00001)
	{
		m_inverseMatrix.identity();
		return m_inverseMatrix;
	}

	float invDet = 1.0 / det;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			m_inverseMatrix.m_data[i * 4 + j] = invDet * getCofator(j,i,m);
		}
	}

	return m_inverseMatrix;
}


