#ifndef DRAWABLE_ENTITY_H
#define DRAWABLE_ENTITY_H

#include <DirectXMath.h>
#include "Entity.h"

using namespace DirectX;

class DrawableEntity : public Entity
{
public:
	DrawableEntity(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);
	~DrawableEntity();

	// Init, Update, Render
	virtual void Render(ID3D11DeviceContext* deviceContext);

private:
	Mesh* mesh;
	Material* material;
	ID3D11RasterizerState* rasterizerState;
	ID3D11DepthStencilState* depthStencilState;
};

#endif