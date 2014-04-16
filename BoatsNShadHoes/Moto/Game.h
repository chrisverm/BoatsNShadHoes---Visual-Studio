#ifndef GAME_H
#define GAME_H

#include "DX.h"
#include "Gameplay.h"

#include <iostream>

class Game : public DX
{
public:
	Game(HINSTANCE hInstance);
	~Game();

	static int Run();
	static void Release();

	static bool Initialize(HINSTANCE hInstance, int iconResource);
	static void OnResize();
	static void Update(float dt);
	static void Draw(float dt);

private:

};

#endif