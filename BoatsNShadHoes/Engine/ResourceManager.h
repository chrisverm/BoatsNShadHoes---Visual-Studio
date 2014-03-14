#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <d3d11.h>

#include "Mesh.h"
#include "Material.h"


typedef std::map<std::string, Mesh*> MeshMap;
typedef std::map<std::string, Material*> MatMap;
typedef std::map<std::string, ID3D11ShaderResourceView*> SRVMap;
typedef std::map<std::string, ID3D11SamplerState*> SSMap;

class ResourceManager
{
public:
	
	static bool AddMesh(std::string, Mesh*);
	static bool AddMaterial(std::string, Material*);
	static bool AddSRV(std::string, ID3D11ShaderResourceView*);
	static bool AddSamplerState(std::string, ID3D11SamplerState*);

	static Mesh* GetMesh(std::string);
	static Material* GetMaterial(std::string);
	static ID3D11ShaderResourceView* GetSRV(std::string);
	static ID3D11SamplerState* GetSamplerState(std::string);

	static void Release();

private:
	static MeshMap meshes;
	static MatMap materials;
	static SRVMap srvs;
	static SSMap samplerStates;

};

#endif