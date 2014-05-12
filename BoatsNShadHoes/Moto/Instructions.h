#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

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
#include "Screen.h"

class Instructions : public GameState
{
public:
	Instructions(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~Instructions();
	bool Initialize();
	void Update(float dt);
	void Draw(float dt);

private:
	Entity* world;
	std::vector<DrawableEntity*> entities;

	void LoadShadersAndInputLayout();
	void LoadResources();

	bool viewChanged;
	bool drawCoordinates;

};

#endif