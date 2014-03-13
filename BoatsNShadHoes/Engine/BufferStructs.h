#ifndef BUFFER_STRUCTS_H
#define BUFFER_STRUCTS_H

#include <DirectXMath.h>

struct VSPerFrameData
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct VSPerModelData
{
	DirectX::XMFLOAT4X4 model;
};

#endif