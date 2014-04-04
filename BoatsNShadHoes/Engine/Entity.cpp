#include "Entity.h"

Entity::Entity()
{
	position = XMVectorSet(0.0f , 0.0f , 0.0f ,0.0f);
	rotation = XMVectorSet(0.0f , 0.0f , 0.0f ,0.0f);
	scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	forward = XMVectorSet(0,0,1.0f,0);
	up = XMVectorSet(0,1.0f,0,0);
	right = XMVectorSet(1.0f,0,0,0);
	
	Forward = &forward;
	Up = &up;
	Right = &right;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

Entity::~Entity() { }

void Entity::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	XMMATRIX worldMat = XMMatrixIdentity();
	
	XMMATRIX trans = XMMatrixTranslationFromVector(position);
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX sca = XMMatrixScalingFromVector(scale);
	worldMat *= sca * rot * trans;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));
	
	UpdateOrientation(worldMat);
}

void Entity::UpdateOrientation()
{
	UpdateOrientation(XMMatrixRotationRollPitchYawFromVector(rotation));
}

void Entity::UpdateOrientation(const XMMATRIX& rot, bool transpose)
{
	// transpose so we can access the columns as xmvectors.
	XMMATRIX r = (transpose) ? XMMatrixTranspose(rot) : rot;

	forward = rot.r[2];
	up = rot.r[1];
	right = rot.r[0];
}