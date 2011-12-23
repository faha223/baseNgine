#include <cmath>
#include "quaternion.h"
using namespace std;

quaternion::quaternion(const float& a, const float& b, const float& c, const float& d)
{	x = a; y = b; z = c; w = d;	}

quaternion::quaternion(const quaternion& quat)
{	x = quat.x; y = quat.y; z = quat.z; w = quat.w;	}
