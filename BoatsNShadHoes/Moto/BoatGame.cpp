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

	ReleaseMacro(vsPerFrameConstantBuffer);
	ReleaseMacro(vsPerModelConstantBuffer);
	ReleaseMacro(vsPerSceneConstantBuffer);

	delete vsPerFrameData;
	delete vsPerModelData;
	delete vsPerSceneData;
	
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
	vsPerSceneData = new VSPerSceneData();

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
	camDesc.InitialPosition = new XMVECTOR(XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f));
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

	// Lighting Setup ----------------------------------
	PointLight pntLight1;
	pntLight1.Range = 2.0f;
	pntLight1.Position = XMFLOAT3(0.5f, -0.5f, -1.5f);
	pntLight1.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	pntLight1.Attenuation = XMFLOAT3(0.0f, 0.2f, 1.0f);

	vsPerSceneData->pntLights[0] = pntLight1;
	deviceContext->UpdateSubresource(
		vsPerSceneConstantBuffer,
		0,
		NULL,
		vsPerSceneData,
		0,
		0);

	return true;
}

void BoatGame::LoadShadersAndInputLayout()
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	// PNU Shaders -------------------------------------
	D3DReadFileToBlob(L"Shaders/VS_PNU.cso", &vsBlob);
	D3DReadFileToBlob(L"Shaders/PS_PNU.cso", &psBlob);

	HR(device->CreateInputLayout(
		vertex_PNU_Desc,
		ARRAYSIZE(vertex_PNU_Desc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	ResourceManager::AddInputLayout("PNU", inputLayout);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("PNU", vertexShader);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));
	ResourceManager::AddPixelShader("PNU", pixelShader);

	ReleaseMacro(vsBlob);
	ReleaseMacro(psBlob);

	// PNC Shaders -------------------------------------
	D3DReadFileToBlob(L"Shaders/VS_PNC.cso", &vsBlob);
	D3DReadFileToBlob(L"Shaders/PS_PNC.cso", &psBlob);

	HR(device->CreateInputLayout(
		vertex_PNC_Desc,
		ARRAYSIZE(vertex_PNC_Desc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	ResourceManager::AddInputLayout("PNC", inputLayout);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("PNC", vertexShader);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));
	ResourceManager::AddPixelShader("PNC", pixelShader);

	ReleaseMacro(vsBlob);
	ReleaseMacro(psBlob);

	// PNUC Shaders ------------------------------------
	D3DReadFileToBlob(L"Shaders/VS_PNUC.cso", &vsBlob);
	D3DReadFileToBlob(L"Shaders/PS_PNUC.cso", &psBlob);

	HR(device->CreateInputLayout(
		vertex_PNUC_Desc,
		ARRAYSIZE(vertex_PNUC_Desc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	ResourceManager::AddInputLayout("PNUC", inputLayout);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("PNUC", vertexShader);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));
	ResourceManager::AddPixelShader("PNUC", pixelShader);

	ReleaseMacro(vsBlob);
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
		&vsPerSceneConstantBuffer));
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
	std::string* vertexType = new std::string();
	Mesh* cube = Mesh::LoadFromOBJ("Resources/boat_obj.obj");
	cube->Initialize(device, ResourceManager::GetInputLayout(cube->ILName()));

	Mesh* quad = Mesh::LoadFromOBJ("Resources/water_obj.obj");
	quad->Initialize(device, ResourceManager::GetInputLayout(quad->ILName()));

	// Begin Disgusting.
	Vertex_PNUC temp[] = 
	{
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,2), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,2,0), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(1,0,0,1) },
		{ XMFLOAT3(2,0,0), XMFLOAT3(0,0,0), XMFLOAT2(0,0), XMFLOAT4(1,0,0,1) }
	};

	Vertex_PNUC* verts = new Vertex_PNUC[6];

	for (int i = 0; i < 6; i++)
	{ verts[i] = temp[i]; }
	
	UINT32 itemp[] = { 0,1,2,3,4,5 };
	UINT32* indices = new UINT[6];
	for (int i = 0; i < 6; i++)
	{ indices[i] = itemp[i]; }
	// End Disgusting.

	Mesh* coordinates = new Mesh(verts, 6, indices, 6);
	coordinates->Initialize(device, ResourceManager::GetInputLayout("PNUC"));

	ResourceManager::AddMesh("coordinates", coordinates);
	ResourceManager::AddMesh("cube", cube);
	ResourceManager::AddMesh("quad", quad);

	// Materials -----------------------------------------
	Material* crateMat = new Material(srv, ss);
	crateMat->Initialize(ResourceManager::GetVertexShader("PNUC"), ResourceManager::GetPixelShader("PNUC"));

	Material* waterMat = new Material(nullptr, nullptr);
	waterMat->Initialize(ResourceManager::GetVertexShader("PNC"), ResourceManager::GetPixelShader("PNC"));

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
	if (Input::KeyUp('I'))
		drawCoordinates = !drawCoordinates;

	switch (state)
	{
		case Menu:
			if (Input::KeyUp('G'))
			{
				state = GameLoop;
				main_bgm->start();
				printf("Now in Game!\n");
			}
			break;
		case GameLoop:
			if (Input::KeyUp('P'))
			{
				printf("Pause\n");
				state = Paused;
			}
			
			// change velocity of audio
			if (Input::KeyUp('Q'))
				main_bgm->changeVelocity(-.0001f);
			if (Input::KeyUp('W'))
				main_bgm->changeVelocity(+.0001f);

			break;
		case Paused:
			if (Input::KeyUp('G'))
			{
				printf("Back to Game.\n");
				state = GameLoop;
			}
			if (Input::KeyUp('M'))
			{
				printf("Back to Menu.\n");
				state = Menu;
			}
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
	//deviceContext->IASetInputLayout(inputLayout);

	deviceContext->VSSetConstantBuffers(0, 1, &vsPerFrameConstantBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &vsPerModelConstantBuffer);
	deviceContext->VSSetConstantBuffers(2, 1, &vsPerSceneConstantBuffer);
	
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