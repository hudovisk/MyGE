
#ifndef VEC2_H
#define VEC2_H

class Vec2
{
public:
	float m_data[2];

	Vec2 operator*(const float rhs) const;
	Vec2 operator+(const Vec2& rhs) const;
	Vec2 operator-(const Vec2& rhs) const;
	Vec2 operator-() const;
	bool operator==(const Vec2& rhs) const;
	Vec2& operator=(const Vec2& rhs);

	float dotProduct(const Vec2& rhs) const;

	float magnitude() const;
	float magnitudeSquared() const;

	Vec2& normalize();

	Vec2& inverse();

	Vec2(float x, float y);
	Vec2();
	Vec2(const Vec2& v);
	~Vec2();
};

Vec2 operator*(const float lhs, const Vec2& rhs);

#endif //VEC2_H