#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DXGame.h"

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

class Game : public DXGame
{
public:
	Game(HINSTANCE hInstance);
	~Game();
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();

	void OnMouseDown(WPARAM, int, int);
	void OnMouseUp(WPARAM, int, int);
	void OnMouseMove(WPARAM, int, int);

protected :
	ID3D11InputLayout* inputLayout;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	virtual void LoadShadersAndInputLayout();

private:
	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

#endif