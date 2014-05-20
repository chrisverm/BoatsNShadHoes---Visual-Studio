#ifndef ENDGAME_H
#define ENDGAME_H

#include "Gamestate.h"

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

class EndGame :
	public GameState
{
public:
	EndGame(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~EndGame(void);
	
	void Unload();
	bool Initialize();
	void Update(float dt);
	void Draw(float dt);

	static bool win;

private:
	Entity* world;
	std::vector<DrawableEntity*> entities;

	void LoadShadersAndInputLayout();
	void LoadResources();

	bool viewChanged;
	bool drawCoordinates;
};

#endif