#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(Vertex_PUNC* verts, int vSize, UINT* inds, int iSize)
{
	vertices = verts;
	numVerts = vSize;
	indices = inds;
	numInds = iSize;

	vertices2.data = nullptr;
}

Mesh::Mesh(VertexArray<void*> verts, int vSize, UINT* inds, int iSize)
{
	vertices2 = verts;
	numVerts = vSize;
	indices = inds;
	numInds = iSize;
}

Mesh::~Mesh()
{
	if (vBuffer != nullptr)
	{
		ReleaseMacro(vBuffer);
		vBuffer = nullptr;
	}
	
	if (iBuffer != nullptr)
	{
		ReleaseMacro(iBuffer);
		iBuffer = nullptr;
	}
	
	if (vertices != nullptr)
	{
		delete[] vertices;
		vertices = nullptr;
	}

	if (indices != nullptr)
	{
		delete[] indices;
		indices = nullptr;
	}
}

void Mesh::Initialize(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	this->inputLayout = inputLayout;

	if (vertices2.data == nullptr)
	{
		// Create the vertex buffer
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex_PUNC) * numVerts; // Number of vertices
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA initialVertexData;
		initialVertexData.pSysMem = vertices;
		HR(device->CreateBuffer(&vbd, &initialVertexData, &vBuffer));
	}
	else
	{
		// Create the vertex buffer
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = vertices2.TotalArrayBytes; // Number of vertices
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA initialVertexData;
		initialVertexData.pSysMem = vertices2.data;
		HR(device->CreateBuffer(&vbd, &initialVertexData, &vBuffer));
	}
	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * numInds; // Number of indices
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
	UINT stride = vertices2.IndividualBytes;
	UINT offset = 0;

	deviceContext->IASetInputLayout(inputLayout);
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

	int numVerts = std::count(data.begin(), data.end(), 'f') * 3;
	int numIndices = std::count(data.begin(), data.end(), 'f') * 3;
	std::string* split = nullptr;

	XMFLOAT3* tempVerts = new XMFLOAT3[numVerts]; int iTV = 0;
	XMFLOAT2* tempUVs   = new XMFLOAT2[numVerts]; int iUV = 0;
	XMFLOAT3* tempNorms = new XMFLOAT3[numVerts]; int iNO = 0;
	XMFLOAT4* tempCols  = new XMFLOAT4[numVerts]; int iCO = 0;
	USHORT* vertIndices = new USHORT[numIndices]; int iIS = 0;
	USHORT* uvIndices	= new USHORT[numIndices];
	USHORT* normIndices = new USHORT[numIndices];
	USHORT* colIndices  = new USHORT[numIndices];

	while (data.length() > 0)
	{
		split = nullptr;
		line = data.substr(0, data.find_first_of('\n'));
		data = data.substr(line.length() + 1, data.length());

		int commented = line.find_first_of('#');
		if (commented >= 0) line = line.substr(0, commented);

		if (line[0] == 'v' && line[1] != 't' && line[1] != 'n' && line[1] != 'c') // v = Vertex
		{
			line = line.substr(2, line.length() - 1);
			split = Split(line, ' ');

			tempVerts[iTV++] = XMFLOAT3((float)atof(split[0].c_str()),
										(float)atof(split[1].c_str()),
										(float)atof(split[2].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 'n') // vn = Vertex Normal
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempNorms[iNO++] = XMFLOAT3((float)atof(split[0].c_str()),
										(float)atof(split[1].c_str()),
										(float)atof(split[2].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 't') // vt = Vertex Texture (UV)
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempUVs[iUV++] = XMFLOAT2((float)atof(split[0].c_str()),
									  (float)atof(split[1].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 'c') // vc = Vertex Color
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempCols[iCO++] = XMFLOAT4((float)atof(split[0].c_str()),
									   (float)atof(split[1].c_str()),
									   (float)atof(split[2].c_str()),
									   (float)atof(split[3].c_str()));
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
				uvIndices[iIS]   = atoi(split2[1].c_str());
				normIndices[iIS] = atoi(split2[2].c_str());
				colIndices[iIS]  = atoi(split2[3].c_str());
				iIS++;

				delete[] split2;
				split2 = nullptr;
			}
		}

		delete[] split;
		split = nullptr;
	}

	Vertex_PUNC* verts = new Vertex_PUNC[numVerts];
	for (int i = 0; i < numVerts; i++)
	{
		Vertex_PUNC v = { tempVerts[vertIndices[i] - 1], tempUVs[uvIndices[i] - 1], 
							tempNorms[normIndices[i] - 1], tempCols[colIndices[i] - 1] };
		verts[i] = v;
	}

	UINT* indices = new UINT[numIndices];
	for (int i = 0; i < numIndices; i++)
	{
		indices[i] = i;
	}

	delete[] tempVerts;
	delete[] tempUVs;
	delete[] tempNorms;
	delete[] tempCols;
	delete[] vertIndices;
	delete[] uvIndices;
	delete[] normIndices;
	delete[] colIndices;

	return new Mesh(verts, numVerts, indices, numIndices);
}

Mesh* Mesh::LoadFromOBJ2(std::string objFilePath)
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

	// remove all comments from file before reading data
	int numComments = std::count(data.begin(), data.end(), '#');
	while (numComments > 0)
	{
		int comment = data.find_first_of('#');

		while (data[comment - 1] == ' ')
			comment--;

		while (data[comment] != '\n')
			data.erase(comment, 1);

		numComments--;
	}

	// seperate vertex data from polygon faces
	std::string faces = data.substr(data.find_first_of('f'), data.length());
	faces = faces.substr(0, faces.find_last_of("0123456789") + 1) + '\n';
	data = data.substr(0, data.length() - faces.length());
	data = data.substr(0, data.find_last_of("0123456789") + 1) + '\n';

	VertexHelper vh;
	vh.Normals	 = std::count(data.begin(), data.end(), 'n');
	vh.UVs		 = std::count(data.begin(), data.end(), 't');
	vh.Colors    = std::count(data.begin(), data.end(), 'c');
	vh.Positions = std::count(data.begin(), data.end(), 'v') - vh.Normals - vh.UVs - vh.Colors;

	int iVerts = 0; XMFLOAT3* tempVerts = (vh.Positions > 0) ? new XMFLOAT3[vh.Positions] : nullptr;
	int iNorms = 0; XMFLOAT3* tempNorms = (vh.Normals > 0) ? new XMFLOAT3[vh.Normals] : nullptr;
	int iUVs   = 0; XMFLOAT2* tempUVs	= (vh.UVs > 0) ? new XMFLOAT2[vh.UVs] : nullptr;
	int iColrs = 0; XMFLOAT4* tempColrs = (vh.Colors > 0) ? new XMFLOAT4[vh.Colors] : nullptr;

	std::string* split = nullptr;

	while (data.length() > 0)
	{
		split = nullptr;
		line = data.substr(0, data.find_first_of('\n'));
		data = data.substr(line.length() + 1, data.length());

		if (line[0] == 'v' && line[1] != 't' && line[1] != 'n' && line[1] != 'c') // v = Vertex
		{
			line = line.substr(2, line.length() - 1);
			split = Split(line, ' ');

			tempVerts[iVerts++] = XMFLOAT3((float)atof(split[0].c_str()),
										   (float)atof(split[1].c_str()),
										   (float)atof(split[2].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 'n') // vn = Vertex Normal
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempNorms[iNorms++] = XMFLOAT3((float)atof(split[0].c_str()),
										   (float)atof(split[1].c_str()),
										   (float)atof(split[2].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 't') // vt = Vertex Texture (UV)
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempUVs[iUVs++] = XMFLOAT2((float)atof(split[0].c_str()),
									   (float)atof(split[1].c_str()));
		}
		else if (line[0] == 'v' && line[1] == 'c') // vc = Vertex Color
		{
			line = line.substr(3, line.length() - 1);
			split = Split(line, ' ');

			tempColrs[iColrs++] = XMFLOAT4((float)atof(split[0].c_str()),
										   (float)atof(split[1].c_str()),
										   (float)atof(split[2].c_str()),
										   (float)atof(split[3].c_str()));
		}
		else
		{
			// there be problems in them hills
		}

		delete[] split;
		split = nullptr;
	}

	int numIndices = std::count(faces.begin(), faces.end(), 'f') * 3; // 3 vertices in each face
	void* vertexData = malloc(numIndices * vh.GetIndividualBytes());
	int tail = 0;

	while (faces.length() > 0)
	{
		line = faces.substr(0, faces.find_first_of('\n'));
		faces = faces.substr(line.length() + 1, faces.length());

		if (line[0] == 'f')
		{
			line = line.substr(2, line.length() - 1); // 2 for "f "
			split = Split(line, ' ');

			std::string* split2 = nullptr;
			int indices = std::count(line.begin(), line.end(), ' ') + 1;
			for (int i = 0, j = 0; i < indices; i++, j = 0)
			{
				split2 = Split(split[i], '/');

				*(XMFLOAT3*)((byte*)vertexData + tail) = tempVerts[atoi(split2[j++].c_str()) - 1];
				tail += sizeof(XMFLOAT3);

				if (vh.UVs)
				{
					*(XMFLOAT2*)((byte*)vertexData + tail) = tempUVs[atoi(split2[j++].c_str()) - 1];
					tail += sizeof(XMFLOAT2);
				}

				if (vh.Normals)
				{
					*(XMFLOAT3*)((byte*)vertexData + tail) = tempNorms[atoi(split2[j++].c_str()) - 1];
					tail += sizeof(XMFLOAT3);
				}

				if (vh.Colors)
				{
					*(XMFLOAT4*)((byte*)vertexData + tail) = tempColrs[atoi(split2[j++].c_str()) - 1];
					tail += sizeof(XMFLOAT4);
				}

				delete[] split2;
				split2 = nullptr;
			}
		}
		else
		{
			// there be problems in them hills
		}

		delete[] split;
		split = nullptr;
	}

	UINT* indices = new UINT[numIndices];
	for (int i = 0; i < numIndices; i++)
	{
		indices[i] = i;
	}

	VertexArray<void*> verts;
	verts.data = vertexData;
	verts.TotalArrayBytes = numIndices * vh.GetIndividualBytes();
	verts.IndividualBytes = vh.GetIndividualBytes();
	verts.inputLayoutDesc = vertex_PUNC_Desc;

	delete[] tempVerts;
	delete[] tempNorms;
	delete[] tempUVs;
	delete[] tempColrs;

	return new Mesh(verts, numIndices, indices, numIndices);
}