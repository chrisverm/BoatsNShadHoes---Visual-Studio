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

Entity::~Entity() 
{
	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end() ; it++)
	{ delete (*it); }
}

void Entity::Update(float dt)
{
	XMMATRIX trans = XMMatrixTranslationFromVector(position);
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX sca = XMMatrixScalingFromVector(scale);
	XMMATRIX worldMat = sca * rot * trans;

	if (parent != nullptr)
	{
		// Load parent (transpose the stored float4x4).
		XMMATRIX parentMat = XMMatrixTranspose(XMLoadFloat4x4(&parent->worldMatrix));
		
		// Multiply on the left side.
		worldMat *= parentMat;
	}

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));

	UpdateOrientation(worldMat);	

	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end(); it++)
	{ (*it)->Update(dt); }
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

int Entity::ChildCount()
{ return children.size(); }

Entity* Entity::AddChild(Entity* child)
{
	//TODO: check if already here.
	children.push_back(child);

	child->parent = this;
	return child;
}

Entity* Entity::GetChild(int index)
{
	return children[index];
}

Entity* Entity::RemoveChild(int index)
{
	Entity* child = children[index];

	children.erase(children.begin() + index);
	child->parent = nullptr;

	return child;
}