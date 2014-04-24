#include "Entity.h"
#include "Camera.h"

Entity::Entity()
	: Forward(forward), Up(up), Right(right)
{
	position = XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f);
	rotation = XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f);
	forward	 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	scale	 = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	up		 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	right	 = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	roll = 0;
	maxRoll = 0;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

Entity::~Entity() 
{
	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end() ; it++)
	{
		if (dynamic_cast<Camera*>(*it) == NULL)
			delete (*it);
	}
}

void Entity::Update(float dt, const XMMATRIX& parentMat)
{
	// clamp maximum amount a ship can roll
	if (roll < -maxRoll)
		roll = -maxRoll;
	else if (roll > maxRoll)
		roll = maxRoll;
	
	// modular arithmetic on roll (no % operator on floats)
	while (roll >= 360) roll -= 360;
	while (roll <= -360) roll += 360;

	rotation = XMVectorSetZ(rotation, XMConvertToRadians(roll));

	XMMATRIX trans = XMMatrixTranslationFromVector(position);
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX sca = XMMatrixScalingFromVector(scale);
	XMMATRIX worldMat = sca * rot * trans * parentMat;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));

	UpdateForwardFromRotation();

	SetUnitVectors();

	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end(); it++)
	{ (*it)->Update(dt, worldMat); }
}

void Entity::UpdateForwardFromRotation()
{
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);

	forward = rot.r[2];
}

void Entity::UpdateRotationFromForward()
{
	// for the love of all that is holy, someone improve this
	// (I don't even know if it works 100%, I think it does but ...?)
	XMVECTOR absoluteForward = XMVectorSet(0, 0, 1, 0);

	XMVECTOR xPlane = XMVectorSet(0, XMVectorGetY(forward), XMVectorGetZ(forward), 0);
	XMVECTOR yPlane = XMVectorSet(XMVectorGetX(forward), 0, XMVectorGetZ(forward), 0);
	float xAngle = XMVectorGetX(XMVector3AngleBetweenVectors(absoluteForward, yPlane));
	float yAngle = XMVectorGetY(XMVector3AngleBetweenVectors(absoluteForward, xPlane));

	rotation = XMVectorSetX(rotation, xAngle);
	rotation = XMVectorSetY(rotation, yAngle);
}

void Entity::SetUnitVectors()
{
	forward = XMVector3Normalize(forward);

	right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
	right = XMVector3Normalize(right);

	up = XMVector3Cross(forward, right);
	up = XMVector3Normalize(up);

	//// roll - rotate on the z axis
	//up = XMVectorSetX(up, sin( XMConvertToRadians(roll) ));
	//up = XMVectorSetY(up, cos( XMConvertToRadians(roll) ));
	//up = XMVector3Normalize(up);
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