#include "Bounds.h"
#include <vector>

#if defined(DEBUG) | defined(_DEBUG)
bool Bounds::drawBounds;
Mesh* Bounds::mesh;
Material* Bounds::mat;
#endif

using namespace DirectX;

XMVECTOR Bounds::verts[8] = { XMVectorSet( 0.5f,  0.5f, 0.5f, 0), XMVectorSet( 0.5f,  0.5f, -0.5f, 0), XMVectorSet( -0.5f,  0.5f, -0.5f, 0), XMVectorSet( -0.5f,  0.5f, 0.5f, 0),
							  XMVectorSet( 0.5f, -0.5f, 0.5f, 0), XMVectorSet( 0.5f, -0.5f, -0.5f, 0), XMVectorSet( -0.5f, -0.5f, -0.5f, 1), XMVectorSet( -0.5f, -0.5f, 0.5f, 0) };
XMVECTOR Bounds::p1[8];
XMVECTOR Bounds::p2[8];

/*
Constructor.
Requires a pointer to the position of the entity / "Center of the mesh" for this bounding box.
Requires an xmfloat2 defining its X and Z extents
*/
Bounds::Bounds(XMVECTOR* positionPtr, XMFLOAT3 boxExtents)
{
	position = positionPtr;
	extents = boxExtents;
}

/*
Checks whether or not this bounding box is intersecting the other boundinx box.
*/
bool Bounds::Intersecting(Bounds* other)
{ return Bounds::Intersecting(this, other); }

/*
Checks whether or not the two bounding boxes are intersecting.
*/
bool Bounds::Intersecting(Bounds* b1, Bounds* b2)
{ 
	XMVECTOR pos = XMVECTOR(*b1->position);
	XMVECTOR otherPos = XMVECTOR(*b2->position);

	XMVECTOR diff = pos - otherPos;
	
	float length = XMVectorGetX(XMVector3LengthSq(diff));
	float maxExtent = b1->extents.x + b1->extents.z + b2->extents.x + b2->extents.z;

	// dont check if we're too far away.
	if (length < maxExtent * maxExtent)
	{
		XMMATRIX m1 = XMMatrixTranspose( XMLoadFloat4x4(&b1->worldMat) );
		XMMATRIX m2 = XMMatrixTranspose( XMLoadFloat4x4(&b2->worldMat) );

		// Transform box coordinates.
		for (int i = 0; i < 8; i++)
		{
			p1[i] = XMVector3TransformCoord(verts[i], m1);
			p2[i] = XMVector3TransformCoord(verts[i], m2);
		}

		// Need to check both ways (can be intersecting but not containing the other box's points).
		// Return true if first one succeeds, otherwise return whether or not the second succeeds.
		if (Bounds::Intersecting(p1, p2))
			return true;
		else
			return Bounds::Intersecting(p2, p1);
	}

	return false;
}

/*
Private, checks whether or not the given array of 8 vertices, which should define a box, intersect.
*/
bool Bounds::Intersecting(XMVECTOR v1[8], DirectX::XMVECTOR v2[8])
{
	XMVECTOR plane = XMVECTOR();
	std::vector<int> indices;
	std::vector<int> remove;

	// Populate the vector with all the indices.
	for (int i = 0; i < 8; i++)
	{ indices.push_back(i);	}

	for (int i = 0; i < 6; i++)
	{
		// define which plane to use.
		switch (i)
		{
			case 0: plane = XMPlaneFromPointNormal(v1[0], XMVector3Normalize( XMVectorSubtract(v1[1], v1[0]) ) ); break;
			case 1: plane = XMPlaneFromPointNormal(v1[0], XMVector3Normalize( XMVectorSubtract(v1[3], v1[0]) ) ); break;
			case 2: plane = XMPlaneFromPointNormal(v1[2], XMVector3Normalize( XMVectorSubtract(v1[1], v1[2]) ) ); break;
			case 3: plane = XMPlaneFromPointNormal(v1[2], XMVector3Normalize( XMVectorSubtract(v1[3], v1[2]) ) ); break;
			case 4: plane = XMPlaneFromPointNormal(v1[0], XMVector3Normalize( XMVectorSubtract(v1[4], v1[0]) ) ); break;
			case 5: plane = XMPlaneFromPointNormal(v1[4], XMVector3Normalize( XMVectorSubtract(v1[0], v1[4]) ) ); break;
		default:
			printf("weird shit went down in bounds \n");
			break;
		}
						
		int j = 0;
		for (std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
		{
			// If infront of the plane, that vert cant be inside.
			if ( 0.0f > XMVectorGetX(XMPlaneDotCoord(plane, v2[*it])) )
			{ remove.push_back(j); }
			j++;
		}

		// Remove the indices we dont need.
		for (int e = remove.size() - 1; e >= 0 ; e--)
		{ indices.erase(indices.begin() + e); }
		remove.clear();

		// If no more indices left, no intersection (unless the other box can detect one).
		if (indices.size() <= 0)
			return false;
	}

	return true;
}