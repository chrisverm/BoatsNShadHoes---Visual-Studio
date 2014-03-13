#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "BufferStructs.h"

class Entity
{
public:
	Entity(Mesh* mesh);
	void Initialize(ID3D11Device* device, ID3D11Buffer* modelConstBuffer);
	void Update(ID3D11DeviceContext* deviceContext, float dt);
	void Render(ID3D11DeviceContext* deviceContext);

	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

private:
	Mesh* mesh;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ID3D11Buffer* modelConstBuffer;
	VSPerModelData modelConstBufferData;

	
	DirectX::XMFLOAT4X4 worldMatrix;
};

