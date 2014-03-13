#include "ResourceManager.h"

MeshMap ResourceManager::meshes;
MatMap ResourceManager::materials;
SRVMap ResourceManager::srvs;
SSMap ResourceManager::samplerStates;

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

bool ResourceManager::AddSRV(std::string id, ID3D11ShaderResourceView* srv)
{
	if (srvs[id] != nullptr)
		return false;

	srvs[id] = srv;

	return true;
}

bool ResourceManager::AddSamplerState(std::string id, ID3D11SamplerState* samplerState)
{
	if (samplerStates[id] != nullptr)
		return false;

	samplerStates[id] = samplerState;

	return true;
}

Mesh* ResourceManager::GetMesh(std::string id)
{ return meshes[id]; }

Material* ResourceManager::GetMaterial(std::string id)
{ return materials[id]; }

ID3D11ShaderResourceView* ResourceManager::GetSRV(std::string id)
{ return srvs[id]; }

ID3D11SamplerState* ResourceManager::GetSamplerState(std::string id)
{ return samplerStates[id]; }

void ResourceManager::Release()
{
	for (MeshMap::iterator it = meshes.begin(); it != meshes.end(); it++)
	{ delete (it->second); }

	for (MatMap::iterator it = materials.begin(); it != materials.end(); it++)
	{ delete (it->second); }

	for (SRVMap::iterator it = srvs.begin(); it != srvs.end(); it++)
	{ ReleaseMacro(it->second); }

	for (SSMap::iterator it = samplerStates.begin(); it != samplerStates.end(); it++)
	{ ReleaseMacro(it->second); }
}
