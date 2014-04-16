#include "Boat.h"
#include "InputManager.h"

using namespace DirectX;
Boat::Boat(std::string meshId, std::string matId) 
	: MoveableEntity(ResourceManager::GetMesh(meshId), ResourceManager::GetMaterial(matId))
{ 
	//angualrVelocity = XMVectorSet(0,1,1,0);
	//velocity = XMVectorSet(0,0.25f,0.5f,0);

	friction = 0.9f;
}

Boat::~Boat()
{ }

void Boat::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	MoveableEntity::Initialize(modelConstBuffer, modelConstBufferData);
}

void Boat::Update(ID3D11DeviceContext* deviceContext, float dt)
{
	MoveableEntity::Update(deviceContext, dt);
}

void Boat::SetPosition(float x, float y, float z)
{
	XMFLOAT3 pos(x, y, z);
	position = XMLoadFloat3(&pos);
}

void Boat::SetRotation(float roll, float pitch, float yaw)
{
	//Yaw Yaw Yaw YEET
	XMFLOAT3 rot(roll, pitch, yaw);
	rotation = XMLoadFloat3(&rot);
	UpdateOrientation();
}

void Boat::MoveForward()
{
	velocity = XMVECTOR(*Forward);
}

void Boat::PortHelm()
{
	angularVelocity = -XMVECTOR(*Up) / 5;
}

void Boat::StarboardHelm()
{
	angularVelocity = XMVECTOR(*Up) / 5;
}