#ifndef SCREEN_H
#define SCREEN_H

#include "DrawableEntity.h"
#include "ResourceManager.h"

class Screen : public DrawableEntity
{
public:
	Screen(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);
	~Screen(void);
};

#endif