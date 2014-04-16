#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <d3d11.h>

class GameState
{
public:
	virtual ~GameState() { }
	virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		this->device = device; // DXGame::device
		this->deviceContext = deviceContext; // DXGame::deviceContext
		return true;
	}
	virtual void Update(float dt) = 0;
	virtual void Draw(float dt) = 0;

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

};

#endif