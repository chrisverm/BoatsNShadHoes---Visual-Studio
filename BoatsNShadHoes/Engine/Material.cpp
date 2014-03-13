#include "Material.h"

Material::Material(ID3D11ShaderResourceView* srv, ID3D11SamplerState* ss)
{
	textured = srv != nullptr;

	this->srv = srv;
	this->ss = ss;
}

Material::~Material() { }

void Material::Initialize(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	this->vShader = vShader;
	this->pShader = pShader;
}

void Material::SetShaders(ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetShader(vShader, NULL, 0);
	deviceContext->PSSetShader(pShader, NULL, 0);

	if (textured)
	{
		deviceContext->PSSetShaderResources(0, 1, &srv);
		deviceContext->PSSetSamplers(0, 1, &ss);
	}
	else
	{
		deviceContext->PSSetShaderResources(0, 0, nullptr);
		deviceContext->PSSetSamplers(0, 0, nullptr);
	}
}
