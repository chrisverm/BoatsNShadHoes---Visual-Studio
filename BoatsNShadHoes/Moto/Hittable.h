#ifndef HITTABLE_H
#define HITTABLE_H

#include "Bounds.h"

__interface Hittable
{
public:
	virtual void Hit(float damage);
	
	virtual Bounds* GetBoundsPtr();
};

#endif

