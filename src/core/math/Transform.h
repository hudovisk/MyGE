
#pragma once

#include "Quaternion.h"
#include "Matrix4.h"
#include "Vec4.h"

class Transform
{
private:
    Quaternion  m_rotation;
    Vec3        m_translation;
    Vec3        m_scale;
    Matrix4     m_matrix;
    Matrix4		m_inverseMatrix;
    bool        m_dirty;
    bool 		m_inverseDirty;

public:
    Transform();
    ~Transform();

    Quaternion getRotation();
    void setRotation(Quaternion quaternion);

    void setRotationInRadian(float angle, Vec3 axis);
    void setRotationInDegree(float angle, Vec3 axis);

    void rotateInRadian(float angle, Vec3 axis);
    void rotateInDegree(float angle, Vec3 axis);

    Vec3 transform(const Vec4& v);

    Vec3 getPosition() { return m_translation; }
    void translate(Vec3 pos);
    void scale(Vec3 scale);

    Matrix4& getMatrix();
    Matrix4& getInverseMatrix();
};
