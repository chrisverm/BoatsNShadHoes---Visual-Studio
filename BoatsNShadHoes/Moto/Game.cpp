#include "Game.h"
#include "InputManager.h"

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	// Attach console window! :D
	// Requires use of _CRT_SECURE_NO_WARNINGS for freopen to work
	// see pre-processor stuff for debug in properties

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
#endif

#ifndef SOUND_PLAY
	//#define SOUND_PLAY
#endif

	// Make the game, initialize and run

	// icon resource
	int iconResource = 101;

	if (!Game::Initialize(hInstance, iconResource))
		return 0;

	return Game::Run();
}

ID3D11Buffer* Game::vsPerModelConstBuffer = nullptr;
ID3D11Buffer* Game::vsPerFrameConstBuffer = nullptr;
ID3D11Buffer* Game::vsPerSceneConstBuffer = nullptr;
VSPerModelData* Game::vsPerModelData = new VSPerModelData();
VSPerFrameData* Game::vsPerFrameData = new VSPerFrameData();
VSPerSceneData* Game::vsPerSceneData = new VSPerSceneData();
bool Game::projChanged = false;

bool Game::Initialize(HINSTANCE hInstance, int iconResource)
{
	hAppInst = hInstance;

	windowCaption = L"MotionOfTheOcean";
	windowWidth = 800;
	windowHeight = 600;

	if (!DX::Initialize(iconResource))
		return false;

	CreateConstantBuffers();

	Instructions* instructions = new Instructions(device, deviceContext);
	Gameplay* gameplay = new Gameplay(device, deviceContext);
	EndGame* endgame = new EndGame(device, deviceContext);
	GameStateManager::AddState("Instructions", instructions);
	GameStateManager::AddState("Gameplay", gameplay);
	GameStateManager::AddState("EndGame", endgame);
	GameStateManager::ChangeState("Instructions");

	return true;
}

void Game::CreateConstantBuffers()
{
	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC vsPerFrameBufferDesc;
	vsPerFrameBufferDesc.ByteWidth = sizeof(*vsPerFrameData);
	vsPerFrameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerFrameBufferDesc.CPUAccessFlags = 0;
	vsPerFrameBufferDesc.MiscFlags = 0;
	vsPerFrameBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerFrameBufferDesc,
		NULL,
		&vsPerFrameConstBuffer));

	D3D11_BUFFER_DESC vsPerModelBufferDesc;
	vsPerModelBufferDesc.ByteWidth = sizeof(*vsPerModelData);
	vsPerModelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerModelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerModelBufferDesc.CPUAccessFlags = 0;
	vsPerModelBufferDesc.MiscFlags = 0;
	vsPerModelBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerModelBufferDesc,
		NULL,
		&vsPerModelConstBuffer));

	D3D11_BUFFER_DESC vsPerSceneBufferDesc;
	vsPerSceneBufferDesc.ByteWidth = sizeof(*vsPerSceneData);
	vsPerSceneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerSceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerSceneBufferDesc.CPUAccessFlags = 0;
	vsPerSceneBufferDesc.MiscFlags = 0;
	vsPerSceneBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerSceneBufferDesc,
		NULL,
		&vsPerSceneConstBuffer));
}

void Game::Release(bool local)
{
	if (!local)	DX::Release();

	if (vsPerModelConstBuffer != nullptr)
	{
		ReleaseMacro(vsPerModelConstBuffer);
		vsPerModelConstBuffer = nullptr;
	}

	if (vsPerFrameConstBuffer != nullptr)
	{
		ReleaseMacro(vsPerFrameConstBuffer);
		vsPerFrameConstBuffer = nullptr;
	}

	if (vsPerSceneConstBuffer != nullptr)
	{
		ReleaseMacro(vsPerSceneConstBuffer);
		vsPerSceneConstBuffer = nullptr;
	}

	if (vsPerModelData != nullptr)
	{
		delete vsPerModelData;
		vsPerModelData = nullptr;
	}

	if (vsPerFrameData != nullptr)
	{
		delete vsPerFrameData;
		vsPerFrameData = nullptr;
	}

	if (vsPerSceneData != nullptr)
	{
		delete vsPerSceneData;
		vsPerSceneData = nullptr;
	}
}

void Game::Reset()
{
	if (vsPerModelData != nullptr)
		delete vsPerModelData;
	vsPerModelData = new VSPerModelData();
	deviceContext->UpdateSubresource(
		vsPerModelConstBuffer,
		0,
		NULL,
		vsPerModelData,
		0,
		0);

	if (vsPerFrameData != nullptr)
		delete vsPerFrameData;
	vsPerFrameData = new VSPerFrameData();
	deviceContext->UpdateSubresource(
		vsPerFrameConstBuffer,
		0,
		NULL,
		vsPerFrameData,
		0,
		0);

	if (vsPerSceneData != nullptr)
		delete vsPerSceneData;
	vsPerSceneData = new VSPerSceneData();
	deviceContext->UpdateSubresource(
		vsPerSceneConstBuffer,
		0,
		NULL,
		vsPerSceneData,
		0,
		0);
}

void Game::OnResize()
{
	DX::OnResize();

	projChanged = true;
}

int Game::Run()
{
	MSG msg = {0};
	timer.Reset();

	// Loop until we get a quit message from windows
	while(msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// No message, so continue the game loop
			timer.Tick();

			if( gamePaused )
			{
				Sleep(100);
			}
			else
			{
				DX::PreUpdate();

				Game::Update(timer.DeltaTime());
				Game::Draw(timer.DeltaTime());

				DX::PostUpdate();
			}
		}
	}

	Release();

	return (int)msg.wParam;
}

void Game::Update(float dt)
{
	(*currentState)->Update(dt);
}

void Game::Draw(float dt)
{
	(*currentState)->Draw(dt);
}