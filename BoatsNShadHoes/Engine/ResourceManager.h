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
typedef std::map<std::string, ID3D11VertexShader*> VSMap;
typedef std::map<std::string, ID3D11PixelShader*> PSMap;
typedef std::map<std::string, ID3D11InputLayout*> ILMap;
typedef std::map<std::string, ID3D11RasterizerState*> RSMap;

class ResourceManager
{
public:
	
	static bool AddMesh(std::string, Mesh*);
	static bool AddMaterial(std::string, Material*);
	static bool AddSRV(std::string, ID3D11ShaderResourceView*);
	static bool AddSamplerState(std::string, ID3D11SamplerState*);
	static bool AddPixelShader(std::string, ID3D11PixelShader*);
	static bool AddVertexShader(std::string, ID3D11VertexShader*);
	static bool AddInputLayout(std::string, ID3D11InputLayout*);
	static bool AddRasterizerState(std::string, ID3D11RasterizerState*);

	static Mesh* GetMesh(std::string);
	static Material* GetMaterial(std::string);
	static ID3D11ShaderResourceView* GetSRV(std::string);
	static ID3D11SamplerState* GetSamplerState(std::string);
	static ID3D11VertexShader* GetVertexShader(std::string);
	static ID3D11PixelShader* GetPixelShader(std::string);
	static ID3D11InputLayout* GetInputLayout(std::string);
	static ID3D11RasterizerState* GetRasterizerState(std::string);

	static void Release();

private:

	static MeshMap meshes;
	static MatMap materials;
	static SRVMap srvs;
	static SSMap samplerStates;
	static VSMap vertexShaders;
	static PSMap pixelShaders;
	static ILMap inputLayouts;
	static RSMap rasterizerStates;
};

// Convienience.
typedef ResourceManager Resources;

#endif