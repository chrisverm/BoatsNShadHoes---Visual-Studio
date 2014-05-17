#ifndef GAMEPLAY_H
#define GAMEPLAY_H

// Uncomment to have audio!
//#define SOUND_PLAY

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
#include "PlayerBoat.h"
#include "AIBoat.h"
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
	PlayerBoat* playerBoat;
	AIBoat* otherBoat;

	void LoadShadersAndInputLayout();
	void LoadResources();
	void SetupAudio();

	// Audio
	ALCdevice* audioDevice;
	ALCcontext* audioDeviceContext;
	AudioPlayer* main_bgm;
	std::map<std::string, AudioPlayer*> sounds;

	bool viewChanged;
	bool boatsColliding; // this should be removed later.
};

#endif