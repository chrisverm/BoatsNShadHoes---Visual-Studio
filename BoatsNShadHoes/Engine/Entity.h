#ifndef ENTITY_H
#define ENTITY_H

#include <DirectXMath.h>
#include <vector>
#include "BufferStructs.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class Entity
{
public:
	Entity();
	~Entity();

	// World
	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;
	float roll;
	float maxRoll;

	// Init, Update, Render
	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(float dt, const XMMATRIX& parentMat);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	// Parenting
	int ChildCount();
	Entity* AddChild(Entity*);
	Entity* GetChild(int);
	Entity* RemoveChild(int);
	Entity* Parent();

	// Orientation
	const XMVECTOR& Forward;
	const XMVECTOR& Up;
	const XMVECTOR& Right;

	void UpdateForwardFromRotation();
	void UpdateRotationFromForward();

	void SetUnitVectors();

protected:
	XMFLOAT4X4 worldMatrix;

	// Orientation
	XMVECTOR forward;
	XMVECTOR up;
	XMVECTOR right;

	// Parenting
	std::vector<Entity*> children;
	Entity* parent;

};

#endif