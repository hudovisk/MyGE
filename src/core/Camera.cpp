
#include "core/Camera.h"

#include <cmath>

Matrix4& Camera::getProjectionMatrix()
{
	if(!m_dirty)
		return m_projectionMatrix;

	float ymax = m_nearDistance * tanf(m_fov/2);
	float xmax = ymax*m_aspectRatio;

	float temp = 2 * m_nearDistance;
	float temp2 = 2 * xmax;
	float temp3 = 2 * ymax;
	float temp4 = m_farDistance - m_nearDistance;

	m_projectionMatrix.m_data[0] = temp / temp2;
	m_projectionMatrix.m_data[1] = 0;
	m_projectionMatrix.m_data[2] = 0;
	m_projectionMatrix.m_data[3] = 0;

	m_projectionMatrix.m_data[4] = 0;
	m_projectionMatrix.m_data[5] = temp / temp3;
	m_projectionMatrix.m_data[6] = 0;
	m_projectionMatrix.m_data[7] = 0;

	m_projectionMatrix.m_data[8] = 0;
	m_projectionMatrix.m_data[9] = 0;
	m_projectionMatrix.m_data[10] = - (m_farDistance + m_nearDistance) / temp4;
	m_projectionMatrix.m_data[11] = -1;//(-temp * m_farDistance) / temp4;

	m_projectionMatrix.m_data[12] = 0;
	m_projectionMatrix.m_data[13] = 0;
	m_projectionMatrix.m_data[14] = (-temp * m_farDistance) / temp4;//-1;
	m_projectionMatrix.m_data[15] = 0;

	m_dirty = false;

	return m_projectionMatrix;
}

Matrix4& Camera::getProjectionMatrix(float fov, float aspectRatio, float nearDistance, float farDistance)
{
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_nearDistance = nearDistance;
	m_farDistance = farDistance;

	m_dirty = true;

	return getProjectionMatrix();
}
