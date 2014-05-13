#include "Water.h"

using namespace DirectX;

Water::Water(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState, ID3D11DepthStencilState* depthStencilState) 
	: DrawableEntity(mesh, material, rasterizerState, depthStencilState)
{
	position = XMVectorSetY(position, -1);
	scale = XMVectorSet(150,1,150,0);
}

Water::~Water(void)
{
}