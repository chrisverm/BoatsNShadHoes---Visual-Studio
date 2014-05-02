#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

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
typedef std::map<std::string, ID3D11DepthStencilState*> DSSMap;

class ResourceManager
{
public:
	static void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	
	static bool AddMesh(std::string, Mesh*);
	static bool AddMaterial(std::string, Material*);
	static bool AddVertexShader(std::string, ID3D11VertexShader*);
	static bool AddPixelShader(std::string, ID3D11PixelShader*);
	static bool AddInputLayout(std::string, ID3D11InputLayout*);
	static bool AddShaderResourceView(std::string, ID3D11ShaderResourceView*);
	static bool AddSamplerState(std::string, ID3D11SamplerState*);
	static bool AddRasterizerState(std::string, ID3D11RasterizerState*);
	static bool AddDepthStencilState(std::string, ID3D11DepthStencilState*);

	static bool CreateMesh(std::string id, std::string objFilePath, ID3D11InputLayout* inputLayout = nullptr);
	static bool CreateMaterial(std::string id, ID3D11ShaderResourceView* shaderResourceView, 
		ID3D11SamplerState* samplerState, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);
	static bool CreateVertexShaderAndInputLayout(std::string id, std::wstring filepath, 
		D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements);
	static bool CreatePixelShader(std::string id, std::wstring filepath);
	static bool CreateShaderResourceView(std::string id, std::wstring textureFilePath);
	static bool CreateSamplerState(std::string id, D3D11_SAMPLER_DESC samplerDesc);
	static bool CreateRasterizerState(std::string id, D3D11_RASTERIZER_DESC rasterizerDesc);
	static bool CreateDepthStencilState(std::string id, D3D11_DEPTH_STENCIL_DESC depthStencilDesc);

	static Mesh* GetMesh(std::string);
	static Material* GetMaterial(std::string);
	static ID3D11ShaderResourceView* GetSRV(std::string);
	static ID3D11SamplerState* GetSamplerState(std::string);
	static ID3D11VertexShader* GetVertexShader(std::string);
	static ID3D11PixelShader* GetPixelShader(std::string);
	static ID3D11InputLayout* GetInputLayout(std::string);
	static ID3D11RasterizerState* GetRasterizerState(std::string);
	static ID3D11DepthStencilState* GetDepthStencilState(std::string);

	static void Release();

private:
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;

	static MeshMap meshes;
	static MatMap materials;
	static SRVMap shaderResourceViews;
	static SSMap samplerStates;
	static VSMap vertexShaders;
	static PSMap pixelShaders;
	static ILMap inputLayouts;
	static RSMap rasterizerStates;
	static DSSMap depthStencilStates;
};

// Convienience.
typedef ResourceManager Resources;

#endif