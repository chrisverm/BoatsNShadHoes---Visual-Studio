#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(Vertex* verts, int vSize, UINT* inds, int iSize)
{
	vertices = verts;
	numVerts = vSize;
	indices = inds;
	numInds = iSize;
}

Mesh::~Mesh()
{

}

void Mesh::Initialize(ID3D11Device* device)
{
	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)* numVerts; // Number of vertices
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &initialVertexData, &vBuffer));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* numInds; // Number of indices
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &initialIndexData, &iBuffer));
}

void Mesh::SetBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
}

Mesh* Mesh::LoadFromOBJ(std::string objFilePath)
{
	std::string data = "";
	std::string line = "";

	try
	{
		std::ifstream file(objFilePath.c_str());
		file.exceptions(std::ifstream::failbit);

		while (!file.eof())
		{
			std::getline(file, line);
			data += line + '\n';
		}

		file.close();
	}
	catch (std::ifstream::failure e)
	{
		// how to catch here?
		std::string errorText = "Could not find " + objFilePath + "\n\n" + e.what() + "\n\n" + e.code().message();

		MessageBox(NULL, std::wstring(errorText.begin(), errorText.end()).c_str(),
			L"File not found error!", MB_ICONEXCLAMATION | MB_OK);
	}

	int numVerts = std::count(data.begin(), data.begin() + data.find("\n\n"), 'v');
	int numIndices = std::count(data.begin(), data.end(), 'f') * 3;
	std::string* split = nullptr;

	XMFLOAT3* tempVerts = new XMFLOAT3[numVerts]; int iTV = 0;
	XMFLOAT2* tempUVs = new XMFLOAT2[numVerts]; int iUV = 0;
	XMFLOAT3* tempNorms = new XMFLOAT3[numVerts]; int iNO = 0;
	USHORT* vertIndices = new USHORT[numIndices]; int iIS = 0;
	//USHORT* uvIndices	= new USHORT[numIndices];
	//USHORT* normIndices = new USHORT[numIndices];

	while (data.length() > 0)
	{
		line = data.substr(0, data.find_first_of('\n'));
		data = data.substr(line.length() + 1, data.length());

		if (line[0] == 'v' && line[1] != 't' && line[1] != 'n') // v = Vertex
		{
			line = line.substr(2, line.length() - 1);
			split = Split(line, ' ');

			tempVerts[iTV++] = XMFLOAT3(atof(split[0].c_str()),
				atof(split[1].c_str()),
				atof(split[2].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 't') // vt = Vertex Texture (UV)
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempUVs[iUV++] = XMFLOAT2(atof(split[0].c_str()),
				atof(split[1].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 'n') // vn = Vertex Normal
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempNorms[iNO++] = XMFLOAT3(atof(split[0].c_str()),
				atof(split[1].c_str()),
				atof(split[2].c_str()));
		}
		else if (line[0] == 'f')
		{
			line = line.substr(2, line.length() - 1);
			split = Split(line, ' ');

			std::string* split2 = nullptr;
			for (int i = 0; i < std::count(line.begin(), line.end(), ' ') + 1; i++)
			{
				split2 = Split(split[i], '/');

				vertIndices[iIS] = atoi(split2[0].c_str());
				//uvIndices[iIS]   = atoi(split2[1].c_str());
				//normIndices[iIS] = atoi(split2[2].c_str());
				iIS++;
			}
		}
	}

	Vertex* verts = new Vertex[numVerts];
	for (int i = 0; i < numVerts; i++)
	{
		Vertex v = { tempVerts[i], XMFLOAT4(1, 1, 1, 1) }; //tempUVs[i], tempNorms[i] };
		verts[i] = v;
	}

	UINT* indices = new UINT[numIndices];
	for (int i = 0; i < numIndices; i++)
	{
		indices[i] = vertIndices[i] - 1;
	}

	return new Mesh(verts, numVerts, indices, numIndices);
}
