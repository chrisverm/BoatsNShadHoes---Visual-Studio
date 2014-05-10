#include "DrawableEntity.h"

#pragma region Constructor/Destructor

/*
Constructor

Usese parents constructor.
Requires a Mesh and a Material to draw with.
*/
DrawableEntity::DrawableEntity(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState)
	: Entity()
{
	this->mesh = mesh;
	this->material = material;
	this->rasterizerState = rasterizerState;
	this->depthStencilState = depthStencilState;
}

/*
Destructor, loops through children and deletes them, (ignores cameras because they're handled in camera manager).
*/
DrawableEntity::~DrawableEntity()
{ Entity::~Entity(); }

#pragma endregion

#pragma region Init, Update, Render

/*
Render this entity ( and all of its children ).
*/
void DrawableEntity::Render(ID3D11DeviceContext* deviceContext)
{
	SetConstantBuffer(deviceContext, worldMatrix);
		
	deviceContext->OMSetDepthStencilState(depthStencilState, 0);
	deviceContext->RSSetState(rasterizerState);

	mesh->SetBuffers(deviceContext);
	material->SetShaders(deviceContext);

	deviceContext->DrawIndexed(
		mesh->numInds,
		0,
		0);

	Entity::Render(deviceContext);
}

#pragma endregion