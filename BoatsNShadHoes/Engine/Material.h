#ifndef MATERIAL_H
#define MATERIAL_H

#include <d3d11.h>
#include "Globals.h"

class Material
{
public:
	Material(ID3D11ShaderResourceView* srv, ID3D11SamplerState* ss);
	~Material();
	void Initialize(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	void SetShaders(ID3D11DeviceContext* deviceContext);

private:
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;

	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* ss;
	bool textured;

};

#endif