#include "DrawableEntity.h"

#pragma region Constructor/Destructor

/*
Constructor

Usese parents constructor.
Requires a Mesh and a Material to draw with.
*/
DrawableEntity::DrawableEntity(Mesh* mesh, Material* material)
	: Entity()
{
	this->mesh = mesh;
	this->material = material;

	topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

/*
Destructor, loops through children and deletes them, (ignores cameras because they're handled in camera manager).
*/
DrawableEntity::~DrawableEntity()
{ Entity::~Entity(); }

#pragma endregion

#pragma region Init, Update, Render

/*
Initialize this entity with the constant buffer to use and the buffer data to pass through.
*/
void DrawableEntity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	this->modelConstBuffer = modelConstBuffer;
	this->modelConstBufferData = modelConstBufferData;
}

/*
Render this entity ( and all of its children ).
*/
void DrawableEntity::Render(ID3D11DeviceContext* deviceContext)
{
	SetConstantBuffer(deviceContext);
		
	deviceContext->IASetPrimitiveTopology(topology);

	mesh->SetBuffers(deviceContext);
	material->SetShaders(deviceContext);

	deviceContext->DrawIndexed(
		mesh->numInds,
		0,
		0);
}

#pragma endregion

/*
Passes this entity's information to the constant buffer.
*/
void DrawableEntity::SetConstantBuffer(ID3D11DeviceContext* deviceContext)
{
	modelConstBufferData->model = worldMatrix;

	deviceContext->UpdateSubresource(
		modelConstBuffer,
		0,
		NULL,
		modelConstBufferData,
		0,
		0);
}