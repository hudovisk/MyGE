
#ifndef SPATIAL_H
#define SPATIAL_H

#include "math/Transform.h"

class Spatial
{
public:
	Spatial() { }
	~Spatial() { }

	Transform m_transform;	
};

#endif //SPATIAL_H