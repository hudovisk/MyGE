
#ifndef COLOR_RGBA_H
#define COLOR_RGBA_H

class ColorRGBA
{
public:
	float m_data[4];

	ColorRGBA operator*(const float rhs) const;
	ColorRGBA operator+(const ColorRGBA& rhs) const;
	ColorRGBA operator-(const ColorRGBA& rhs) const;
	ColorRGBA operator-() const;
	bool operator==(const ColorRGBA& rhs) const;
	ColorRGBA& operator=(const ColorRGBA& rhs);

	ColorRGBA(float r, float g, float b, float a);
	ColorRGBA();
	ColorRGBA(const ColorRGBA& v);
	~ColorRGBA();
};


ColorRGBA operator*(const float lhs, const ColorRGBA& rhs);

#endif