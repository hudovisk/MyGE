
#ifndef CAMERA_H
#define CAMERA_H

#include "math/Matrix4.h"

const float PIdiv180 = 0.017453;

class Camera
{
public:
	Camera() :
		m_fov(90 * PIdiv180), m_nearDistance(1), m_farDistance(10),
		m_aspectRatio(1), m_dirty(true), m_projectionMatrix(Matrix4())
	{
	}

	Camera(float fov, float nearDistance, float farDistance)
		: m_fov(fov), m_nearDistance(nearDistance), m_farDistance(farDistance),
		  m_aspectRatio(1), m_dirty(true), m_projectionMatrix(Matrix4())
	{
	}

	~Camera() { destroy(); }

	bool init() { return true; }

	bool destroy() { return true; }

	Matrix4& getProjectionMatrix();

	Matrix4& getProjectionMatrix(float fov, float aspectRatio, float nearDistance, float farDistance);

	void setFov(float fov) { m_dirty = true; m_fov = fov * PIdiv180; }
	void setNearDistance(float nearDistance) { m_dirty = true; m_nearDistance = nearDistance; }
	void setFarDistance(float farDistance) { m_dirty = true; m_farDistance = farDistance; }
	void setAspectRatio(float aspectRatio) { m_dirty = true; m_aspectRatio = aspectRatio; }
	float getFov() { return m_fov; }
	float getNearDistance() { return m_nearDistance; }
	float getFarDistance() { return m_farDistance; }

private:
	float m_fov;
	float m_nearDistance;
	float m_farDistance;
	float m_aspectRatio;
	bool m_dirty;
	Matrix4 m_projectionMatrix;
};

#endif //CAMERA_H