#include "Bounds.h"
#include <vector>

#if defined(DEBUG) | defined(_DEBUG)
bool Bounds::drawBounds;
Mesh* Bounds::mesh;
Material* Bounds::mat;
#endif

using namespace DirectX;

XMVECTOR Bounds::verts[8] = { XMVectorSet( 0.5, 0.5, 0.5,1), XMVectorSet( 0.5, 0.5, -0.5,1), XMVectorSet( -0.5, 0.5, -0.5,1), XMVectorSet( -0.5, 0.5, 0.5, 1),
						XMVectorSet( 0.5, -0.5, 0.5, 1), XMVectorSet( 0.5, -0.5, -0.5, 1), XMVectorSet( -0.5, -0.5, -0.5, 1), XMVectorSet( -0.5, -0.5, 0.5, 1) };
XMVECTOR Bounds::p1[8];
XMVECTOR Bounds::p2[8];

Bounds::Bounds(XMVECTOR* positionPtr, XMFLOAT2 boxExtents)
{
	position = positionPtr;
	extents = boxExtents;
}

bool Bounds::Intersecting(Bounds* b1, Bounds* b2)
{ return b1->Intersecting(b2); }

bool Bounds::Intersecting(Bounds* other, bool first)
{
	XMVECTOR pos = XMVECTOR(*position);
	XMVECTOR otherPos = XMVECTOR(*other->position);

	XMVECTOR diff = pos - otherPos;
	
	float length = XMVectorGetX(XMVector3LengthSq(diff));
	float maxExtent = extents.x + extents.y + other->extents.x + other->extents.y;

	if (length < maxExtent * maxExtent)
	{
		XMMATRIX m1 = XMMatrixTranspose( XMLoadFloat4x4(&worldMat) );
		XMMATRIX m2 = XMMatrixTranspose( XMLoadFloat4x4(&other->worldMat) );

		std::vector<int> indices;
		std::vector<int> remove;
		for (int i = 0; i < 8; i++)
		{
			indices.push_back(i);
			p1[i] = XMVector3TransformCoord(verts[i], m1);
			p2[i] = XMVector3TransformCoord(verts[i], m2);
		}

		XMVECTOR plane = XMVECTOR();

		for (int i = 0; i < 6; i++)
		{
			if (i == 0)
				plane = XMPlaneFromPointNormal(p1[0], XMVector3Normalize( XMVectorSubtract(p1[1], p1[0]) ) );
			else if (i == 1)
				plane = XMPlaneFromPointNormal(p1[0], XMVector3Normalize( XMVectorSubtract(p1[3], p1[0]) ) );
			else if (i == 2)
				plane = XMPlaneFromPointNormal(p1[2], XMVector3Normalize( XMVectorSubtract(p1[1], p1[2]) ) );
			else if (i == 3)
				plane = XMPlaneFromPointNormal(p1[2], XMVector3Normalize( XMVectorSubtract(p1[3], p1[2]) ) );
			else if (i == 4)
				plane = XMPlaneFromPointNormal(p1[0], XMVector3Normalize( XMVectorSubtract(p1[5], p1[0]) ) );
			else // i == 5
				plane = XMPlaneFromPointNormal(p1[5], XMVector3Normalize( XMVectorSubtract(p1[0], p1[5]) ) );
			
			int j = 0;
			for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
			{
				if ( 0.0f >= XMVectorGetX(XMPlaneDotCoord(plane, p2[*it])) )
				{ remove.push_back(j); }
				j++;
			}
			for (int e = remove.size() - 1; e >= 0 ; e--)
			{
				indices.erase(indices.begin() + e);
			}
			remove.clear();

			if (indices.size() <= 0)
				return (first) ? other->Intersecting(this, false) : false;
		}

		return true;
	}
	return false;
}