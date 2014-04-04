#ifndef ENTITY_H
#define ENTITY_H

#include <DirectXMath.h>
#include "BufferStructs.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class Entity
{
public:
	Entity();
	~Entity();

	virtual void Update(ID3D11DeviceContext* deviceContext, float dt);

	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

	void UpdateOrientation();
	void UpdateOrientation(const DirectX::XMMATRIX&, bool = true);

	const DirectX::XMVECTOR* Forward;
	const DirectX::XMVECTOR* Up;
	const DirectX::XMVECTOR* Right;

protected:
	DirectX::XMFLOAT4X4 worldMatrix;

	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR right;
};

#endif