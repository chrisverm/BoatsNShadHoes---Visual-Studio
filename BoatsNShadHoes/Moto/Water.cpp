#include "Water.h"

using namespace DirectX;

Water::Water(Mesh* mesh, Material* material, ID3D11RasterizerState* rasterizerState) 
	: DrawableEntity(mesh, material, rasterizerState)
{
	position = XMVectorSetY(position, -1);
	scale = XMVectorSet(50,1,50,0);
}

Water::~Water(void)
{
}