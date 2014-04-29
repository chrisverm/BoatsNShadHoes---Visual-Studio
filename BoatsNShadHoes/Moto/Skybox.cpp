#include "Skybox.h"

/*
This is a skybox, renders infinitely far away from the camera (100 units actualy), (and actualy its from the boat, not the camera).

GL_FAN has got nothin on this now.
*/
SkyBox::SkyBox(Mesh* mesh, Material* mat, ID3D11RasterizerState* rastState, ID3D11DepthStencilState* depthStenState)
	: DrawableEntity(mesh, mat, rastState, depthStenState)
{
	scale *= 5000.0f;
}
/*
Update the skybox, ignore rotations.
*/
void SkyBox::Update(float dt, const XMMATRIX& parentWorldMat)
{
	// Call this for base draw stuff, but then overwrite worldMat.
	DrawableEntity::Update(dt, parentWorldMat);

	XMMATRIX translation = XMMatrixTranslationFromVector(parent->position);
	XMMATRIX scaleMat = XMMatrixScalingFromVector(scale);

	XMMATRIX world = scaleMat * translation;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
}

