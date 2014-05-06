#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "GameState.h"
#include "Globals.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Entity.h"
#include "AudioPlayer.h"
#include "AudioManager.h"
#include "Boat.h"
#include "Water.h"
#include "CannonBall.h"
#include "Camera.h"
#include "Skybox.h"

class Gameplay : public GameState
{
public:
	Gameplay(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Gameplay();
	bool Initialize();
	void Update(float dt);
	void Draw(float dt);

private:
	Entity* world;
	Boat* playerBoat;
	Boat* otherBoat;

	void LoadShadersAndInputLayout();
	void CreateGeometryBuffers();
	void LoadResources();
	void SetupAudio();

	// Audio
	ALCdevice* audioDevice;
	ALCcontext* audioDeviceContext;
	AudioPlayer* main_bgm;

	bool viewChanged;

};

#endif