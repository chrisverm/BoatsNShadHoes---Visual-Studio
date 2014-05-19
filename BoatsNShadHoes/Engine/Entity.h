#ifndef ENTITY_H
#define ENTITY_H

#include <DirectXMath.h>
#include <vector>
#include "BufferStructs.h"
#include "Mesh.h"
#include "Material.h"
#include "Bounds.h"
#include "Random.h"

// Some people frown upon this being here.
using namespace DirectX;

class Entity
{
public:
	Entity();
	virtual ~Entity();

	ID3D11Buffer* modelConstBuffer;
	VSPerModelData* modelConstBufferData;

	// World
	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;

	// Bounds
	Bounds* bounds;

	// Init, Update, Render
	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(float dt, const XMMATRIX& parentMat);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	//Debug Render
#if defined(DEBUG) | defined(_DEBUG)
	virtual void DebugRender(ID3D11DeviceContext*);
	static bool drawCoordinates;
	static Mesh* coordinateMesh;
	static Material* coordinateMaterial;
#endif

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

	void SetConstantBuffer(ID3D11DeviceContext*, XMFLOAT4X4);

protected:
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4 tint;

	// Orientation
	XMVECTOR forward;
	XMVECTOR up;
	XMVECTOR right;

	// Min/Max Rotation Clamps
	XMVECTOR minAngleClamps;
	XMVECTOR maxAngleClamps;

	// Parenting
	std::vector<Entity*> children;
	Entity* parent;

};

#endif