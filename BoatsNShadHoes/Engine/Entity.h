#ifndef ENTITY_H
#define ENTITY_H

#include <DirectXMath.h>
#include "BufferStructs.h"
#include "Mesh.h"
#include "Material.h"

class Entity
{
public:
	Entity(Mesh* mesh, Material* material);
	~Entity();

	virtual void Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	virtual void Update(ID3D11DeviceContext* deviceContext, float dt);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

	void UpdateOrientation();
	void UpdateOrientation(const DirectX::XMMATRIX&, bool = true);

	const DirectX::XMVECTOR* Forward;
	const DirectX::XMVECTOR* Up;
	const DirectX::XMVECTOR* Right;

	void setCB(ID3D11DeviceContext*);

private:
	Mesh* mesh;
	Material* material;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ID3D11Buffer* modelConstBuffer;
	VSPerModelData* modelConstBufferData;

	DirectX::XMFLOAT4X4 worldMatrix;

	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR right;
};

#endif