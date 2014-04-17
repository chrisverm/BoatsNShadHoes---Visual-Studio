#ifndef GAME_H
#define GAME_H

#include "DX.h"
#include "BufferStructs.h"
#include "Gameplay.h"

class Game : public DX
{
	friend class Gameplay;
public:
	static int Run();
	static void Release(bool local = false);
	static void Reset();

	static bool Initialize(HINSTANCE hInstance, int iconResource);
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

};

#endif