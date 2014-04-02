#ifndef BOAT_GAME_H
#define BOAT_GAME_H

#include <vector>
#include <d3dcompiler.h>
#include "WICTextureLoader.h"
#include "DXGame.h"
#include "Globals.h"
#include "BufferStructs.h"
#include "Boat.h"
#include "Water.h"
#include "Mesh.h"
#include "ResourceManager.h"

#include "Camera.h"
#include "CameraManager.h"

#include "AudioPlayer.h"
#include "AudioManager.h"

enum GameState
{
	Menu,
	GameLoop,
	Paused
};

class BoatGame : public DXGame
{
public:
	BoatGame(HINSTANCE hInstance);
	~BoatGame();

	bool Init(int=0);
	void OnResize();
	void DrawScene();
	void UpdateScene(float);

	GameState state;

protected:

	ID3D11Buffer* vsPerFrameConstantBuffer;
	ID3D11Buffer* vsPerModelConstantBuffer;
	VSPerFrameData* vsPerFrameData;
	VSPerModelData* vsPerModelData;

	void LoadShadersAndInputLayout();
	void CreateGeometryBuffers();
	void LoadResources();

	std::vector<Entity*> entities;

	// Audio
	ALCdevice* audioDevice;
	ALCcontext* audioDeviceContext;

	AudioPlayer* main_bgm;

	bool viewChanged;
	bool projChanged;
	bool drawCoordinates;
};

#endif