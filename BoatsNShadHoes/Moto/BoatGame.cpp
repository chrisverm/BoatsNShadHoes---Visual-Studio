#include "BoatGame.h"

using namespace DirectX;

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

	// Make the game, initialize and run
	BoatGame game(hInstance);

	if (!game.Init())
		return 0;

	return game.Run();
}

BoatGame::BoatGame(HINSTANCE hInstance) : DXGame(hInstance)
{
	windowCaption = L"MotionOfTheOcean";
	windowWidth = 800;
	windowHeight = 600;

	state = Menu;
}


BoatGame::~BoatGame()
{
	DXGame::~DXGame();

	ReleaseMacro(vertexShader);
	ReleaseMacro(texturePixelShader);
	ReleaseMacro(colorPixelShader);
	ReleaseMacro(inputLayout);

	ReleaseMacro(vsPerFrameConstantBuffer);
	ReleaseMacro(vsPerModelConstantBuffer);

	delete vsPerFrameData;
	delete vsPerModelData;
	
	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end() ; it++)
	{ delete (*it); }

	ResourceManager::Release();
	CameraManager::Release();
}

bool BoatGame::Init()
{
	vsPerFrameData = new VSPerFrameData();
	vsPerModelData = new VSPerModelData();

	if (!DXGame::Init())
		return false;

	viewChanged = false;
	CameraManager::Initialize(deviceContext, 1, &windowWidth, &windowHeight, &viewChanged, &projChanged);

	Camera::Mount mount = { Camera::Mount::State::STATIC, Camera::Mount::State::STATIC, Camera::Mount::State::STATIC };
	Camera* camera = new Camera(mount);
	*camera->position = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	*camera->forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	CameraManager::AddCamera(camera, true);

	LoadShadersAndInputLayout();
	CreateGeometryBuffers();
	LoadResources();

	// Entites -----------------------------------------
	Boat* boat = new Boat("cube", "crate");
	boat->Initialize(device, vsPerModelConstantBuffer, vsPerModelData);

	Water* water = new Water("quad", "water");
	water->Initialize(device, vsPerModelConstantBuffer, vsPerModelData);

	entities.push_back(boat);
	entities.push_back(water);

	return true;
}

void BoatGame::LoadShadersAndInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 20,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Load Vertex Shader --------------------------------------
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(L"Shaders/Texture_VertexShader.cso", &vsBlob);

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));

	// Before cleaning up the data, create the input layout
	HR(device->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));

	// Clean up
	ReleaseMacro(vsBlob);

	// Load Pixel Shader ---------------------------------------
	ID3DBlob* psBlob;
	D3DReadFileToBlob(L"Shaders/Texture_PixelShader.cso", &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&texturePixelShader));

	// Clean up
	ReleaseMacro(psBlob);

	D3DReadFileToBlob(L"Shaders/Color_PixelShader.cso", &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&colorPixelShader));

	ReleaseMacro(psBlob);
}

void BoatGame::CreateGeometryBuffers()
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
		&vsPerFrameConstantBuffer));

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
		&vsPerModelConstantBuffer));
}

void BoatGame::LoadResources()
{
	// Shader Resource Views -----------------------------
	ID3D11ShaderResourceView* srv = nullptr;
	HR(CreateWICTextureFromFile(
		device, 
		deviceContext, 
		L"Resources/crate_texture.png", 
		0, 
		&srv));
	ResourceManager::AddSRV("crate", srv);

	// Sampler States ------------------------------------
	ID3D11SamplerState* ss = nullptr;
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	HR(device->CreateSamplerState(
		&samplerDesc,
		&ss));
	ResourceManager::AddSamplerState("crate", ss);

	// Meshes -------------------------------------------
	Mesh* cube = Mesh::LoadFromOBJ("Resources/crate_obj.obj");
	cube->Initialize(device);

	Mesh* quad = Mesh::LoadFromOBJ("Resources/water_obj.obj");
	quad->Initialize(device);

	ResourceManager::AddMesh("cube", cube);
	ResourceManager::AddMesh("quad", quad);

	// Materials -----------------------------------------
	Material* crateMat = new Material(srv, ss);
	crateMat->Initialize(vertexShader, texturePixelShader);

	Material* waterMat = new Material(nullptr, nullptr);
	waterMat->Initialize(vertexShader, colorPixelShader);

	ResourceManager::AddMaterial("crate", crateMat);
	ResourceManager::AddMaterial("water", waterMat);
}

void BoatGame::OnResize()
{
	// Handle base-level DX resize stuff
	DXGame::OnResize();

	projChanged = true;
}

void BoatGame::UpdateScene(float dt)
{
	CameraManager::Update();

	vsPerFrameData->view	   = CameraManager::ActiveCamera()->GetViewMatrix();
	vsPerFrameData->projection = CameraManager::ActiveCamera()->GetProjMatrix();

	deviceContext->UpdateSubresource(
		vsPerFrameConstantBuffer,
		0,
		NULL,
		vsPerFrameData,
		0,
		0);

	(entities[0])->rotation = XMVectorSetX((entities[0])->rotation, XMVectorGetX(entities[0]->rotation) + dt);
	(entities[0])->rotation = XMVectorSetZ((entities[0])->rotation, XMVectorGetZ(entities[0]->rotation) + dt);

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

	// Set up the input 
	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->VSSetConstantBuffers(0, 1, &vsPerFrameConstantBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &vsPerModelConstantBuffer);
	
	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		(*it)->Render(deviceContext);
	}
	
	// Present the buffer
	HR(swapChain->Present(0, 0));
}