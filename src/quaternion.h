#ifndef _QUATERNION_H_
#define _QUATERNION_H_

struct quaternion
{
	quaternion(const quaternion&);
	quaternion(const float& = 0.0f, const float& = 0.0f, const float& = 0.0f, const float& = 0.0f);
	float x, y, z, w;

};

#endif
