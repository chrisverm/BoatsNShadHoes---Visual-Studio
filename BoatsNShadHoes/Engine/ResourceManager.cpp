#include "ResourceManager.h"

ID3D11Device* ResourceManager::device = nullptr;
ID3D11DeviceContext* ResourceManager::deviceContext = nullptr;

MeshMap ResourceManager::meshes;
MatMap ResourceManager::materials;
SRVMap ResourceManager::shaderResourceViews;
SSMap ResourceManager::samplerStates;
PSMap ResourceManager::pixelShaders;
VSMap ResourceManager::vertexShaders;
ILMap ResourceManager::inputLayouts;
RSMap ResourceManager::rasterizerStates;
DSSMap ResourceManager::depthStencilStates;

void ResourceManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ResourceManager::device = device;
	ResourceManager::deviceContext = deviceContext;
}

bool ResourceManager::AddMesh(std::string id, Mesh* mesh)
{
	if (meshes[id] != nullptr)
		return false;

	meshes[id] = mesh;

	return true;
}

bool ResourceManager::AddMaterial(std::string id, Material* material)
{
	if (materials[id] != nullptr)
		return false;

	materials[id] = material;

	return true;
}

bool ResourceManager::AddVertexShader(std::string id, ID3D11VertexShader* vertexShader)
{
	// check if vertex shader with this ID already exists
	if (vertexShaders[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A vertex shader with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	// check if vertex shader param is a nullptr
	if (vertexShader == nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"The vertex shader passed with the ID \"" + wid + L"\" cannot be a nullptr.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	vertexShaders[id] = vertexShader;

	return true;
}

bool ResourceManager::AddPixelShader(std::string id, ID3D11PixelShader* pixelShader)
{
	// check if pixel shader with this ID already exists
	if (pixelShaders[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A pixel shader with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	// check if pixel shader param is a nullptr
	if (pixelShader == nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"The pixel shader passed with the ID \"" + wid + L"\" cannot be a nullptr.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	pixelShaders[id] = pixelShader;

	return true;
}

bool ResourceManager::AddInputLayout(std::string id, ID3D11InputLayout* inputLayout)
{
	// check if input layout with this ID already exists
	if (inputLayouts[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"An input layout with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	// check if input layout param is a nullptr
	if (inputLayout == nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"The input layout passed with the ID \"" + wid + L"\" cannot be a nullptr.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	inputLayouts[id] = inputLayout;

	return true;
}

bool ResourceManager::AddShaderResourceView(std::string id, ID3D11ShaderResourceView* shaderResourceView)
{
	// check if shader resource view with this ID already exists
	if (shaderResourceViews[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A shader resource view with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	// check if shader resource view param is a nullptr
	if (shaderResourceView == nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"The shader resource view passed with the ID \"" + wid + L"\" cannot be a nullptr.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	shaderResourceViews[id] = shaderResourceView;

	return true;
}

bool ResourceManager::AddSamplerState(std::string id, ID3D11SamplerState* samplerState)
{
	// check if sampler state with this ID already exists
	if (samplerStates[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A sampler state with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	// check if sampler state param is a nullptr
	if (samplerState == nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"The sampler state passed with the ID \"" + wid + L"\" cannot be a nullptr.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	samplerStates[id] = samplerState;

	return true;
}

bool ResourceManager::AddRasterizerState(std::string id, ID3D11RasterizerState* rasterizerState)
{
	if (rasterizerStates[id] != nullptr)
		return false;

	rasterizerStates[id] = rasterizerState;

	return true;
}

bool ResourceManager::AddDepthStencilState(std::string id, ID3D11DepthStencilState* depthStencilState)
{
	if (depthStencilStates[id] != nullptr)
		return false;

	depthStencilStates[id] = depthStencilState;

	return true;
}

bool ResourceManager::CreateVertexShaderAndInputLayout(std::string id, std::wstring filepath, 
			D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements)
{
	// check if vertex shader with this ID already exists
	if (vertexShaders[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A vertex shader with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	ID3DBlob* vsBlob = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

	D3DReadFileToBlob(filepath.c_str(), &vsBlob);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vShader));
	AddVertexShader(id, vShader);

	HR(device->CreateInputLayout(
		layoutDesc,
		numElements,
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	AddInputLayout(id, inputLayout);

	ReleaseMacro(vsBlob);

	return true;
}

bool ResourceManager::CreatePixelShader(std::string id, std::wstring filepath)
{
	// check if pixel shader with this ID already exists
	if (pixelShaders[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A pixel shader with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	ID3DBlob* psBlob = nullptr;
	ID3D11PixelShader* pShader = nullptr;

	D3DReadFileToBlob(filepath.c_str(), &psBlob);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pShader));
	AddPixelShader(id, pShader);

	ReleaseMacro(psBlob);

	return true;
}

bool ResourceManager::CreateShaderResourceView(std::string id, std::wstring textureFilePath)
{
	// check if shader resource view with this ID already exists
	if (shaderResourceViews[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A shader resource view with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}

	ID3D11ShaderResourceView* shaderResourceView = nullptr;

	if (textureFilePath.substr(textureFilePath.length() - 4, textureFilePath.length()) == L".dds")
	{
		HR(DirectX::CreateDDSTextureFromFile(
			device, 
			textureFilePath.c_str(), 
			0, 
			&shaderResourceView));
	}
	else
	{
		HR(DirectX::CreateWICTextureFromFile(
			device, 
			deviceContext, 
			textureFilePath.c_str(), 
			0, 
			&shaderResourceView));
	}
	
	AddShaderResourceView(id, shaderResourceView);

	return true;
}

bool ResourceManager::CreateSamplerState(std::string id, D3D11_SAMPLER_DESC samplerDesc)
{
	// check if sampler state with this ID already exists
	if (shaderResourceViews[id] != nullptr)
	{
		std::wstring wid = std::wstring(id.begin(), id.end());
		std::wstring error = L"A sampler state with the ID \"" + wid + L"\" already exists.";
		DXTRACE_ERR_MSGBOX(error.c_str(), NULL);
		return false;
	}
	
	ID3D11SamplerState* ss = nullptr;
	
	HR(device->CreateSamplerState(
		&samplerDesc,
		&ss));
	AddSamplerState(id, ss);

	return true;
}

Mesh* ResourceManager::GetMesh(std::string id)
{ return meshes[id]; }

Material* ResourceManager::GetMaterial(std::string id)
{ return materials[id]; }

ID3D11ShaderResourceView* ResourceManager::GetSRV(std::string id)
{ return shaderResourceViews[id]; }

ID3D11SamplerState* ResourceManager::GetSamplerState(std::string id)
{ return samplerStates[id]; }

ID3D11PixelShader* ResourceManager::GetPixelShader(std::string id)
{ return pixelShaders[id]; }

ID3D11VertexShader* ResourceManager::GetVertexShader(std::string id)
{ return vertexShaders[id]; }

ID3D11InputLayout* ResourceManager::GetInputLayout(std::string id)
{ return inputLayouts[id]; }

ID3D11RasterizerState* ResourceManager::GetRasterizerState(std::string id)
{ return rasterizerStates[id]; }

ID3D11DepthStencilState* ResourceManager::GetDepthStencilState(std::string id)
{ return depthStencilStates[id]; }

void ResourceManager::Release()
{
	for (MeshMap::iterator it = meshes.begin(); it != meshes.end(); it++)
	{ delete (it->second); }

	for (MatMap::iterator it = materials.begin(); it != materials.end(); it++)
	{ delete (it->second); }

	for (SRVMap::iterator it = shaderResourceViews.begin(); it != shaderResourceViews.end(); it++)
	{ ReleaseMacro(it->second); }

	for (SSMap::iterator it = samplerStates.begin(); it != samplerStates.end(); it++)
	{ ReleaseMacro(it->second); }

	for (VSMap::iterator it = vertexShaders.begin(); it != vertexShaders.end(); it++)
	{ ReleaseMacro(it->second); }

	for (PSMap::iterator it = pixelShaders.begin(); it != pixelShaders.end(); it++)
	{ ReleaseMacro(it->second); }

	for (ILMap::iterator it = inputLayouts.begin(); it != inputLayouts.end(); it++)
	{ ReleaseMacro(it->second); }

	for (RSMap::iterator it = rasterizerStates.begin(); it != rasterizerStates.end(); it++)
	{ ReleaseMacro(it->second); }

	for (DSSMap::iterator it = depthStencilStates.begin(); it != depthStencilStates.end(); it++)
	{ ReleaseMacro(it->second); }
}
