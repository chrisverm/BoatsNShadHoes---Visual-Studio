#ifndef MESH_H
#define MESH_H

#include <d3d11.h>
#include <list>
#include "Globals.h"
#include "Vertex.h"
#include <fstream>

class Mesh
{
	friend class Entity;
public:
	Mesh(Vertex[], int vSize, UINT inds[], int iSize);
	~Mesh();
	void Initialize(ID3D11Device* device);
	void SetBuffers(ID3D11DeviceContext* deviceContext);

	static Mesh* LoadFromOBJ(std::string objFilePath);

private:
	int numVerts, numInds;
	Vertex* vertices;
	UINT* indices;
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

};

#endif