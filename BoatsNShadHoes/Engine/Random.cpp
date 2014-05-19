#include "Random.h"

using namespace DirectX;

bool Random::seeded;

/*
Gets a random float between the min and max!
*/
float Random::Range(float min, float max)
{
	if (!seeded) { srand(time(NULL)); seeded = true; }
	return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

/*
Gets a random direction (clamped to a random value between min and max length).
You can specify whether or not you want randomness in the x,y,z,or w components individualy (w defaults to false, rest to true).
*/
XMVECTOR Random::Direction(float minLength,float maxLength, bool x, bool y, bool z, bool w)
{
	XMVECTOR dir =  XMVector4Normalize(
		XMVectorSet(
			(x) ? Range(-1.0f,1.0f) : 0.0f, 
			(y) ? Range(-1.0f,1.0f) : 0.0f, 
			(z) ? Range(-1.0f,1.0f) : 0.0f, 
			(w) ? Range(-1.0f,1.0f) : 0.0f)
		);
	return dir * Range(minLength, maxLength);
}