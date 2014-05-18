#ifndef BUFFER_STRUCTS_H
#define BUFFER_STRUCTS_H

#include <DirectXMath.h>
#include "Lights.h"

#define NUM_DIR_LIGHTS 0
#define NUM_PNT_LIGHTS 1
#define NUM_SPT_LIGHTS 0

struct VSPerFrameData
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	float time;
	DirectX::XMFLOAT3 cameraPosition;
};

struct VSPerModelData
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4 tint;
};

struct VSPerSceneData
{
	//DirectionalLight dLight[NUM_DIR_LIGHTS];
	PointLight pntLights[NUM_PNT_LIGHTS];
	//SpotLight sLight[NUM_SPT_LIGHTS];
};

#endif