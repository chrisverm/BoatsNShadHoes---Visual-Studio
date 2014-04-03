#ifndef LIGHTS_H
#define LIGHTS_H

#include <DirectXMath.h>

struct Light
{
	Light()
	{
		Ambient  = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		Diffuse  = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		Specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
};

struct DirectionalLight : Light
{
	DirectX::XMFLOAT3 Direction;
	float padding; // included if we wantd an array of these lights
};

struct PointLight : Light
{
	float Range;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Attenuation;
	float padding; // included if we wantd an array of these lights
};

struct SpotLight : Light
{
	float Range;
	float Spot;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Direction;
	DirectX::XMFLOAT3 Attenuation;
	float padding; // included if we wantd an array of these lights
};

#endif