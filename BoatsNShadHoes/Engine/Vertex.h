#ifndef VERTEX_H
#define VERTEX_H

#include <DirectXMath.h>

struct Vertex_PNU // are these even needed?
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
};

static D3D11_INPUT_ELEMENT_DESC vertex_PNU_Desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct Vertex_PNC // are these even needed?
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
};

static D3D11_INPUT_ELEMENT_DESC vertex_PNC_Desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct Vertex_PNUC // are these even needed?
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
};

static D3D11_INPUT_ELEMENT_DESC vertex_PNUC_Desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct Vertex_Water
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Offset;
};

static D3D11_INPUT_ELEMENT_DESC vertex_Water_Desc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

struct Vertex_Skybox
{
	DirectX::XMFLOAT3 Position;
};

static D3D11_INPUT_ELEMENT_DESC vertex_Skybox_Desc[] = 
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0}
};

struct VertexArray
{
	void* data;
	UINT TotalArrayBytes;
	UINT IndividualBytes;
	std::string InputLayoutName;
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