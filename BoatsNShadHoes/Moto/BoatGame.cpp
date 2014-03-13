
#include "BoatGame.h"

using namespace DirectX;

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Make the game, initialize and run
	BoatGame game(hInstance);

	if (!game.Init())
		return 0;

	return game.Run();
}

BoatGame::BoatGame(HINSTANCE hInstance) : Game(hInstance)
{
	windowCaption = L"MotionOfTheOcean";
	windowWidth = 800;
	windowHeight = 600;

	state = Menu;
}


BoatGame::~BoatGame()
{
	Game::~Game();

	ReleaseMacro(vsPerDrawConstantBuffer);
	ReleaseMacro(vsPerModelConstantBuffer);

	ReleaseMacro(vertex);
	ReleaseMacro(index);

	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end() ; it++)
	{
		delete (*it);
	}

	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		ReleaseMacro((*it)->iBuffer);
		ReleaseMacro((*it)->vBuffer);
		delete (*it);
	}
}

bool BoatGame::Init()
{
	XMVECTOR position = XMVectorSet(0, 0, -5, 0);
	XMVECTOR target = XMVectorSet(0, 0, 0, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&vsPerDrawData.view, XMMatrixTranspose(V));

	// Set up world matrix
	// In an actual game, update this when the object moves (so every frame)
	//XMMATRIX W = XMMatrixIdentity();
	//XMStoreFloat4x4(&vsPerDrawData.worl, XMMatrixTranspose(W));

	return Game::Init();
}

void BoatGame::OnResize()
{
	// Handle base-level DX resize stuff
	Game::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&vsPerDrawData.projection, XMMatrixTranspose(P));

	XMStoreFloat4x4(&vsPerModelData.model, XMMatrixTranspose(XMMatrixIdentity()));
}



void BoatGame::LoadShadersAndInputLayout()
{
	Game::LoadShadersAndInputLayout();

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC vsPerDrawBufferDesc;
	vsPerDrawBufferDesc.ByteWidth = sizeof(vsPerDrawData);
	vsPerDrawBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerDrawBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerDrawBufferDesc.CPUAccessFlags = 0;
	vsPerDrawBufferDesc.MiscFlags = 0;
	vsPerDrawBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerDrawBufferDesc,
		NULL,
		&vsPerDrawConstantBuffer));

	D3D11_BUFFER_DESC vsPerModelBufferDesc;
	vsPerModelBufferDesc.ByteWidth = sizeof(vsPerModelData);
	vsPerModelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerModelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerModelBufferDesc.CPUAccessFlags = 0;
	vsPerModelBufferDesc.MiscFlags = 0;
	vsPerModelBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerModelBufferDesc,
		NULL,
		&vsPerModelConstantBuffer));


	// SET UP TRIANGLE ----------------------------------------
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices
	Vertex vertices[] =
	{
		{ XMFLOAT3(+1.0f, 0.0f, + 1.0f), blue },
		{ XMFLOAT3(-1.0f, 0.0f, + 1.0f), blue },
		{ XMFLOAT3(+1.0f, 0.0f, - 1.0f), blue },
		{ XMFLOAT3(-1.0f, 0.0f, - 1.0f), blue },
	};

	// Set up the indices
	UINT indices[] = { 3, 1, 0, 2, 3, 0 };

	Mesh* quad = new Mesh(vertices, 4, indices, 6);
	quad->Initialize(device);
	Mesh* cube = Mesh::LoadFromOBJ("Crate.txt");
	cube->Initialize(device);

	meshes.push_back(quad);
	meshes.push_back(cube);

	Boat* boat = new Boat(cube);
	boat->Initialize(device, vsPerModelConstantBuffer);
	Entity* water = new Entity(quad);
	water->Initialize(device, vsPerModelConstantBuffer);

	water->scale = XMVectorSetX(water->scale, 100);
	water->scale = XMVectorSetZ(water->scale, 100);
	water->position = XMVectorSet(0,-1,0,0);

	entities.push_back(boat);
	entities.push_back(water);
}

void BoatGame::UpdateScene(float dt)
{
	(entities[0])->rotation = XMVectorSetX((entities[0])->rotation, XMVectorGetX(entities[0]->rotation) + dt);
	(entities[0])->rotation = XMVectorSetY((entities[0])->rotation, XMVectorGetY(entities[0]->rotation) + dt);
	switch (state)
	{
		case Menu:
			if (GetAsyncKeyState('G'))
				state = GameLoop;
			break;
		case GameLoop:
			if (GetAsyncKeyState('P'))
				state = Paused;
			break;
		case Paused:
			if (GetAsyncKeyState('G'))
				state = GameLoop;
			if (GetAsyncKeyState('M'))
				state = Menu;
			break;
		default:
			break;
	}

	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		(*it)->Update(deviceContext, dt);
	}
}

void BoatGame::DrawScene()
{
	
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set up the input assembler
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(
		0,	// Corresponds to the constant buffer's register in the vertex shader
		1,
		&vsPerDrawConstantBuffer);
	deviceContext->VSSetConstantBuffers(
		1,	// Corresponds to the constant buffer's register in the vertex shader
		1,
		&vsPerModelConstantBuffer);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->UpdateSubresource(
		vsPerDrawConstantBuffer,
		0,
		NULL,
		&vsPerDrawData,
		0,
		0);
	
	deviceContext->UpdateSubresource(
		vsPerModelConstantBuffer,
		0,
		NULL,
		&vsPerModelData,
		0,
		0);

	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		(*it)->Render(deviceContext);
	}

	// Finally do the actual drawing
	

	// Present the buffer
	HR(swapChain->Present(0, 0));
	
}


