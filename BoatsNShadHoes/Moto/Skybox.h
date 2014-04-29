#ifndef BOATSNSHADHOES_SKYBOX_H
#define BOATSNSHADHOES_SKYBOX_H

#include "DrawableEntity.h"

class SkyBox : public DrawableEntity
{
public:
	SkyBox(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);

	void Update(float, const XMMATRIX&);

private:

};

#endif