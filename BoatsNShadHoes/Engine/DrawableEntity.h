#ifndef DRAWABLE_ENTITY_H
#define DRAWABLE_ENTITY_H

#include <DirectXMath.h>
#include "Entity.h"

using namespace DirectX;

class DrawableEntity : public Entity
{
public:
	DrawableEntity(Mesh* mesh, Material* material);
	~DrawableEntity();

	// Init, Update, Render
	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	void SetConstantBuffer(ID3D11DeviceContext*);

private:
	Mesh* mesh;
	Material* material;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ID3D11Buffer* modelConstBuffer;
	VSPerModelData* modelConstBufferData;

};

#endif