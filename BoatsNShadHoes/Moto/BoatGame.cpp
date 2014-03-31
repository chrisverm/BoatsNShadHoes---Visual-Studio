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

	// icon resource
	int iconResource = 101;

	if (!game.Init(iconResource))
		return 0;

	return game.Run();
}

BoatGame::BoatGame(HINSTANCE hInstance) : DXGame(hInstance)
{
	windowCaption = L"MotionOfTheOcean";
	windowWidth = 800;
	windowHeight = 600;

	state = Menu;
	drawCoordinates = false;
}


BoatGame::~BoatGame()
{
	DXGame::~DXGame();
	
	// Close cmd window.
	_fcloseall();

	ReleaseMacro(inputLayout);
	ReleaseMacro(vsPerFrameConstantBuffer);
	ReleaseMacro(vsPerModelConstantBuffer);

	delete vsPerFrameData;
	delete vsPerModelData;
	
	for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end() ; it++)
	{ delete (*it); }

	alcDestroyContext(audioDeviceContext);
	alcCloseDevice(audioDevice);

	delete main_bgm;

	ResourceManager::Release();
	CameraManager::Release();

	// Show memory leaks at end of program
#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}

bool BoatGame::Init(int iconResource)
{
	vsPerFrameData = new VSPerFrameData();
	vsPerModelData = new VSPerModelData();

	if (!DXGame::Init(iconResource))
		return false;

	LoadShadersAndInputLayout();
	CreateGeometryBuffers();
	LoadResources();

	// Entites -----------------------------------------
	Boat* boat = new Boat("cube", "crate");
	boat->Initialize(vsPerModelConstantBuffer, vsPerModelData);

	Water* water = new Water("quad", "water");
	water->Initialize(vsPerModelConstantBuffer, vsPerModelData);

	entities.push_back(boat);
	entities.push_back(water);

	// Camera Setup -----------[ o]---------------------
	viewChanged = false;
	CameraManager::Initialize(deviceContext, 1, &windowWidth, &windowHeight, &viewChanged, &projChanged);

	CAMERA_DESC camDesc;
	camDesc.FieldOfView = XMConvertToRadians(45.0f);
	camDesc.NearPlane = 0.1f;
	camDesc.FarPlane = 100.0f;
	camDesc.InitialRoll = new float(0.0f);
	camDesc.InitialPosition = new XMVECTOR(XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f));
	camDesc.InitialForward = new XMVECTOR(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	camDesc.Position = STATIC;
	camDesc.Forward = STATIC;
	camDesc.Roll = STATIC;
	CameraManager::CreateNewCamera(&camDesc, true);

	// AL setup --------------------------------~^^~-----

	// grab audio device
	audioDevice = alcOpenDevice(NULL);
#ifdef DEBUG
	assert(audioDevice != NULL);
#endif

	// create context for device
	audioDeviceContext = alcCreateContext(audioDevice, NULL);
#ifdef DEBUG
	assert(audioDeviceContext != NULL);
#endif

	// assign context to 'current'
	alcMakeContextCurrent(audioDeviceContext);

	float listenerOrientation[] = {
		XMVectorGetX(*CameraManager::ActiveCamera()->forward),
		XMVectorGetY(*CameraManager::ActiveCamera()->forward),
		XMVectorGetZ(*CameraManager::ActiveCamera()->forward),
		XMVectorGetX(*CameraManager::ActiveCamera()->up),
		XMVectorGetY(*CameraManager::ActiveCamera()->up),
		XMVectorGetZ(*CameraManager::ActiveCamera()->up)
	};
	
	// listener setup! :D
	alListener3f(AL_POSITION, XMVectorGetX(*CameraManager::ActiveCamera()->position), 
							  XMVectorGetY(*CameraManager::ActiveCamera()->position), 
							  XMVectorGetZ(*CameraManager::ActiveCamera()->position));
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, listenerOrientation);

#ifdef DEBUG
	std::cout << "listener position:" << "[" << XMVectorGetX(*CameraManager::ActiveCamera()->position) 
									  << ", " << XMVectorGetY(*CameraManager::ActiveCamera()->position) 
									  << ", " << XMVectorGetZ(*CameraManager::ActiveCamera()->position) << "]" << std::endl;
#endif

	// ^ using assert to replace the following:
	/*audioDevice = alcOpenDevice(NULL);

#ifdef DEBUG
	if(audioDevice == NULL)
		std::cout << "cannot open sound card!" << std::endl;
#endif

	// device accessed successfully
	else
	{
		// create context for device
		audioDeviceContext = alcCreateContext(audioDevice, NULL);

#ifdef DEBUG
		if(audioDeviceContext == NULL)
			std::cout << "cannot create context for our device!" << std::endl;
#endif

		// context created successfully
		else
			alcMakeContextCurrent(audioDeviceContext); // assign our created context to 'current'
	}*/

	// Audio ---------------------d[-_-]b-----------------
	main_bgm = new AudioPlayer("Resources/Zedd-Clarity_BGM.ogg");

	main_bgm->init();
	main_bgm->generateBufferData();
	main_bgm->setLooping(AL_TRUE);

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
	D3DReadFileToBlob(L"Shaders/VS_TexturedColored.cso", &vsBlob);

	ID3D11VertexShader* vertexShader;
	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("Texture", vertexShader);

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
	D3DReadFileToBlob(L"Shaders/PS_TexturedColored.cso", &psBlob);

	ID3D11PixelShader* texturePixelShader;
	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&texturePixelShader));
	ResourceManager::AddPixelShader("Texture", texturePixelShader);

	// Clean up
	ReleaseMacro(psBlob);

	D3DReadFileToBlob(L"Shaders/PS_Colored.cso", &psBlob);

	ID3D11PixelShader* colorPixelShader;
	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&colorPixelShader));
	ResourceManager::AddPixelShader("Color", colorPixelShader);

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

	// Begin Disgusting.
	Vertex temp[] = 
	{
		{ XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,2), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,2,0), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(1,0,0,1) },
		{ XMFLOAT3(2,0,0), XMFLOAT2(0,0), XMFLOAT3(0,0,0), XMFLOAT4(1,0,0,1) }
	};

	Vertex* verts = new Vertex[6];

	for (int i = 0; i < 6; i++)
	{ verts[i] = temp[i]; }
	
	UINT32 itemp[] = { 0,1,2,3,4,5 };
	UINT32* indices = new UINT[6];
	for (int i = 0; i < 6; i++)
	{ indices[i] = itemp[i]; }
	// End Disgusting.

	Mesh* coordinates = new Mesh(verts, 6, indices, 6);
	coordinates->Initialize(device);

	ResourceManager::AddMesh("coordinates", coordinates);
	ResourceManager::AddMesh("cube", cube);
	ResourceManager::AddMesh("quad", quad);

	// Materials -----------------------------------------
	Material* crateMat = new Material(srv, ss);
	crateMat->Initialize(ResourceManager::GetVertexShader("Texture"), ResourceManager::GetPixelShader("Texture"));

	Material* waterMat = new Material(nullptr, nullptr);
	waterMat->Initialize(ResourceManager::GetVertexShader("Texture"), ResourceManager::GetPixelShader("Color"));

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

	// Toggle drawing entities coordinates in debug.
	// TODO: Needs to be set to KeyUp once we have that implemented.
	if (GetAsyncKeyState('I'))
		drawCoordinates = !drawCoordinates;

	switch (state)
	{
		case Menu:
			if (GetAsyncKeyState('G'))
			{
				state = GameLoop;
				main_bgm->start();
			}
			break;
		case GameLoop:
			if (GetAsyncKeyState('P'))
				state = Paused;
			
			// change velocity of audio
			if (GetAsyncKeyState('Q'))
				main_bgm->changeVelocity(-.0001f);
			if (GetAsyncKeyState('W'))
				main_bgm->changeVelocity(+.0001f);

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

	float listenerPos[3];
	alGetListenerfv(AL_POSITION, listenerPos);
	main_bgm->updateGain(listenerPos);
	main_bgm->update();
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

#if defined(DEBUG) | defined(_DEBUG)
	if (drawCoordinates)
	{
		ResourceManager::GetMaterial("water")->SetShaders(deviceContext);
		Mesh* coords = ResourceManager::GetMesh("coordinates");
		coords->SetBuffers(deviceContext);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		for (std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); it++)
		{ 
			(*it)->setCB(deviceContext); 

			deviceContext->DrawIndexed(
			coords->Indices(),
			0,
			0);
		}
	}
	
#endif
	
	// Present the buffer
	HR(swapChain->Present(0, 0));
}