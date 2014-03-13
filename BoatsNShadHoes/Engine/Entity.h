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
	void Initialize(ID3D11Device* device, ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData);
	void Update(ID3D11DeviceContext* deviceContext, float dt);
	void Render(ID3D11DeviceContext* deviceContext);

	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

private:
	Mesh* mesh;
	Material* material;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ID3D11Buffer* modelConstBuffer;
	VSPerModelData* modelConstBufferData;

	
	DirectX::XMFLOAT4X4 worldMatrix;

};

#endif