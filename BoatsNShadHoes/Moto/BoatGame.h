#pragma once

#include "DXGame.h"
#include "Game.h"
#include "BufferStructs.h"
#include "Boat.h"
#include "Mesh.h"
#include <vector>


enum GameState
{
	Menu,
	GameLoop,
	Paused
};

class BoatGame : public Game
{
public:
	BoatGame(HINSTANCE hInstance);
	~BoatGame();

	bool Init();
	void OnResize();
	void DrawScene();
	void UpdateScene(float);

	GameState state;

protected:

	ID3D11Buffer* vsPerDrawConstantBuffer;
	ID3D11Buffer* vsPerModelConstantBuffer;
	VSPerDrawData vsPerDrawData; VSPerModelData vsPerModelData;

	void LoadShadersAndInputLayout();
	void CreateGeometryBuffers();

	ID3D11Buffer* vertex;
	ID3D11Buffer* index;

	std::vector<Entity*> entities;
	std::vector<Mesh*> meshes;
};

