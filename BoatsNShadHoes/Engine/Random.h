#ifndef BOATSNSHADHOES_RANDOM_H
#define BOATSNSHADHOES_RANDOM_H

#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>

class Random
{
public:
	static float Range(float, float);
	static DirectX::XMVECTOR Direction(float=1,float=1, bool=true, bool=true, bool=true, bool=false);
private:
	static bool seeded;
};

#endif
