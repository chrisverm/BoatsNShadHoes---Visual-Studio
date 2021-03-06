#ifndef MESH_H
#define MESH_H

#include <d3d11.h>
#include <list>
#include "Globals.h"
#include "Vertex.h"
#include <fstream>

using namespace DirectX;

class Mesh
{
	friend class DrawableEntity;
public:
	Mesh(VertexArray verts, int vSize, UINT inds[], int iSize, D3D_PRIMITIVE_TOPOLOGY topology);
	~Mesh();
	void Initialize(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	void SetBuffers(ID3D11DeviceContext* deviceContext);

	int Indices() { return numVerts; }
	int Verticies() { return numInds; }
	std::string ILName() { return vertices.InputLayoutName; }

	static Mesh* LoadFromOBJ(std::string objFilePath);

private:
	UINT numVerts, numInds;
	size_t vertexByteSize;
	VertexArray vertices;
	UINT* indices;

	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	ID3D11InputLayout* inputLayout;
	D3D_PRIMITIVE_TOPOLOGY topology;

};

#endif