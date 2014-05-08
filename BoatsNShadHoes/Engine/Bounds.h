#ifndef BOUNDS_H
#define BOUNDS_H

#if defined(DEBUG) | defined(_DEBUG)
#include "Mesh.h"
#include "Material.h"
#endif

#include <DirectXMath.h>

class Bounds
{
public:
	Bounds(DirectX::XMVECTOR*, DirectX::XMFLOAT2);

	DirectX::XMVECTOR* position;
	DirectX::XMFLOAT2 extents;

	static bool Intersecting(Bounds*, Bounds*);

#if defined(DEBUG) | defined(_DEBUG)
	static bool drawBounds;
	static Mesh* mesh;
	static Material* mat;
#endif

	DirectX::XMFLOAT4X4 worldMat;

private:
	bool Intersecting(Bounds*, bool = true);
	static DirectX::XMVECTOR verts[8];
	
	static DirectX::XMVECTOR p1[8];
	static DirectX::XMVECTOR p2[8];
};

#endif

