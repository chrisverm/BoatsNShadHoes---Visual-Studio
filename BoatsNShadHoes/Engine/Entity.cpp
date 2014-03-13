#include "Entity.h"

using namespace DirectX;

Entity::Entity(Mesh* mesh)
{
	this->mesh = mesh;
	//this->material = material;

	topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	position = XMVectorSet(0,0,0,0);// XMFLOAT3(0.0f, 0.0f, 0.0f); //XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMVectorSet(0.0f, 0.0f, 0.0f,0); //XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMVectorSet(1.0f, 1.0f, 1.0f,0); //XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

void Entity::Initialize(ID3D11Device* device, ID3D11Buffer* modelConstBuffer) //VertexModelConstantBuffer* modelConstBufferData)
{
	this->modelConstBuffer = modelConstBuffer;
	//this->modelConstBufferData = modelConstBufferData;
}

void Entity::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	XMMATRIX worldMat = XMMatrixIdentity();
	
	XMMATRIX trans = XMMatrixTranslationFromVector(position);
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX sca = XMMatrixScalingFromVector(scale);
	worldMat *= sca * rot * trans;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));
}

void Entity::Render(ID3D11DeviceContext* deviceContext)
{
	modelConstBufferData.model = worldMatrix;

	deviceContext->UpdateSubresource(
		modelConstBuffer,
		0,
		NULL,
		&modelConstBufferData,
		0,
		0);
		
	deviceContext->IASetPrimitiveTopology(topology);

	mesh->SetBuffers(deviceContext);
	//material->SetShaders(deviceContext);

	deviceContext->DrawIndexed(
		mesh->numInds,
		0,
		0);
}