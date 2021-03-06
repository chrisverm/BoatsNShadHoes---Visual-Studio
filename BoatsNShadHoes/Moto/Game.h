#ifndef GAME_H
#define GAME_H

#include "DX.h"
#include "BufferStructs.h"
#include "Instructions.h"
#include "Gameplay.h"
#include "EndGame.h"

class Game : public DX
{
	friend class Instructions;
	friend class Gameplay;
	friend class EndGame;
	friend class Boat;
	friend class CannonBall;
public:
	// Main game loop
	static int Run();
	// if local, wont release memory in DX
	static void Release(bool local = false);
	// Zeros-out data in buffers 
	static void Reset();

	static bool Initialize(HINSTANCE hInstance, int iconResource);
	static void CreateConstantBuffers();
	static void OnResize();
	static void Update(float dt);
	static void Draw(float dt);

private:
	static ID3D11Buffer* vsPerModelConstBuffer;
	static ID3D11Buffer* vsPerFrameConstBuffer;
	static ID3D11Buffer* vsPerSceneConstBuffer;
	static VSPerModelData* vsPerModelData;
	static VSPerFrameData* vsPerFrameData;
	static VSPerSceneData* vsPerSceneData;

	static bool projChanged;

};

#endif