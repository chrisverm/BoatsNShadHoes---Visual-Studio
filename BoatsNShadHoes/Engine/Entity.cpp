#include "Entity.h"

using namespace DirectX;

Entity::Entity(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	this->material = material;

	topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	position = XMVectorSet(0.0f , 0.0f , 0.0f ,0.0f);
	rotation = XMVectorSet(0.0f , 0.0f , 0.0f ,0.0f);
	scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

Entity::~Entity()
{

}

void Entity::Initialize(ID3D11Device* device, ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	this->modelConstBuffer = modelConstBuffer;
	this->modelConstBufferData = modelConstBufferData;
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
	modelConstBufferData->model = worldMatrix;

	deviceContext->UpdateSubresource(
		modelConstBuffer,
		0,
		NULL,
		modelConstBufferData,
		0,
		0);
		
	deviceContext->IASetPrimitiveTopology(topology);

	mesh->SetBuffers(deviceContext);
	material->SetShaders(deviceContext);

	deviceContext->DrawIndexed(
		mesh->numInds,
		0,
		0);
}

XMVECTOR Entity::getRight()
{
	//XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(1,0,0,0), XMLoadFloat4x4(&worldMatrix)));
	return XMVectorSet(worldMatrix._11, worldMatrix._12, worldMatrix._13, 0);
}

XMVECTOR Entity::getUp()
{
	//XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0,1,0,0), XMLoadFloat4x4(&worldMatrix)));
	return XMVectorSet(worldMatrix._21, worldMatrix._22, worldMatrix._23, 0);
}

XMVECTOR Entity::getForward()
{
	//XMVector3Normalize(XMVector3TransformCoord(XMVectorSet(0,0,1,0), XMLoadFloat4x4(&worldMatrix)));
	return XMVectorSet(worldMatrix._31, worldMatrix._32, worldMatrix._33, 0);
}
