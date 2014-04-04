#include "DrawableEntity.h"

DrawableEntity::DrawableEntity(Mesh* mesh, Material* material)
	: Entity()
{
	this->mesh = mesh;
	this->material = material;

	topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

DrawableEntity::~DrawableEntity()
{
	Entity::~Entity();
}

void DrawableEntity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	this->modelConstBuffer = modelConstBuffer;
	this->modelConstBufferData = modelConstBufferData;
}

void DrawableEntity::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	Entity::Update(deviceContext, dt);
}

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