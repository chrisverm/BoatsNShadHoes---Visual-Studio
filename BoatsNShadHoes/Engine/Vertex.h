#ifndef VERTEX_H
#define VERTEX_H

#include <DirectXMath.h>

struct Vertex_PUNC
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
};

static D3D11_INPUT_ELEMENT_DESC vertex_PUNC_Desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 20,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

template <class T>
struct VertexArray
{
	T data;
	UINT TotalArrayBytes;
	UINT IndividualBytes;
	D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc;
};

struct VertexHelper
{
	int Positions;
	int Normals;
	int UVs;
	int Colors;

	int GetIndividualBytes()
	{
		return (Positions != 0) * sizeof(DirectX::XMFLOAT3) + (Normals != 0) * sizeof(DirectX::XMFLOAT3) +
				(UVs != 0) * sizeof(DirectX::XMFLOAT2) + (Colors != 0) * sizeof(DirectX::XMFLOAT4);
	}
};

#endif