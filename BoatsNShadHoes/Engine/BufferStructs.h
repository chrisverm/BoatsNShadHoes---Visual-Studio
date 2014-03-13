#pragma once

#include <DirectXMath.h>

struct VSPerDrawData
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct VSPerModelData
{
	DirectX::XMFLOAT4X4 model;
};