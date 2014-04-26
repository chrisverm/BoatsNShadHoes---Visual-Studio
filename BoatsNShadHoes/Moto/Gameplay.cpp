#include "Gameplay.h"

Gameplay::Gameplay(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameState(device, deviceContext)
{
	world = new Entity();
	drawCoordinates = false;
}

Gameplay::~Gameplay()
{
#ifdef SOUND_PLAY
	alcDestroyContext(audioDeviceContext);
	alcCloseDevice(audioDevice);

	delete main_bgm;
#endif

	// every entity is somehow connected to this
	// thus this will destroy all entities
	delete world;
}

bool Gameplay::Initialize()
{
	LoadShadersAndInputLayout();
	CreateGeometryBuffers();
	LoadResources();

	Input::SetCursorVisibility(false);
	Input::SetCursorLocking(true);

	// stats for first boat
	BOAT_STATS b1Stats;
	b1Stats.health		= 100.0f;
	b1Stats.armor		= 10.0f;
	b1Stats.ammunition	= 25;
	b1Stats.rateOfFire	= 0.7f;
	b1Stats.damage		= 20;

	// Entites -----------------------------------------
	Boat* boat = new Boat(Resources::GetMesh("cube"), Resources::GetMaterial("crate"), b1Stats, true);
	boat->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	//boat->SetStats(b1Stats);

	// stats for second boat
	BOAT_STATS b2Stats;
	b2Stats.health		= 50.0f;
	b2Stats.armor		= 15.0f;
	b2Stats.ammunition	= 5;
	b2Stats.rateOfFire	= 1.5f;
	b2Stats.damage		= 10;

	Boat* boat2 = new Boat(Resources::GetMesh("cube"), Resources::GetMaterial("crate"), b2Stats, false);
	boat2->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
	boat2->SetPosition(5, 0, 5);
	boat2->SetRotation(0, 2, 0);

	//boat2->SetStats(b2Stats);

	CannonBall* cannonBall = new CannonBall("sphere", "water");
	cannonBall->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
	cannonBall->position = XMVectorSet(0,-10,0,0);

	Water* water = new Water(Resources::GetMesh("quad"), Resources::GetMaterial("water"));
	water->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	entities.push_back(boat);		world->AddChild(boat);
	entities.push_back(boat2);		world->AddChild(boat2);
	entities.push_back(water);		world->AddChild(water);
	entities.push_back(cannonBall);	world->AddChild(cannonBall);

	// Camera Setup -----------[ o]---------------------
	viewChanged = false;
	CameraManager::Initialize(deviceContext, 1, &DX::windowWidth, &DX::windowHeight, &viewChanged, &Game::projChanged);
	
	CAMERA_DESC camDesc;
	camDesc.FieldOfView = XMConvertToRadians(45.0f);
	camDesc.NearPlane = 0.1f;
	camDesc.FarPlane = 100.0f;
	camDesc.AttachedDist = 20.0f;
	camDesc.Parent = boat;
	camDesc.InitialRoll = 0.0f;
	camDesc.InitialPosition = XMVectorSet(0.0f, 7.0f, -15.0f, 0.0f);
	camDesc.InitialForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camDesc.Position = STATIC;
	camDesc.Forward = THIRD_PERSON;
	camDesc.Roll = STATIC;
	CameraManager::CreateNewCamera(&camDesc, true);

#ifdef SOUND_PLAY
	// AL setup --------------------------------~^^~-----
	setupAudio();

	// Audio ---------------------d[-_-]b-----------------
	main_bgm = new AudioPlayer("Resources/Zedd-Clarity_BGM.ogg");

	main_bgm->init();
	//main_bgm->generateBufferData();
	main_bgm->setLooping(AL_TRUE);
#endif

	// Lighting Setup ----------------------------------
	PointLight pntLight1;
	pntLight1.Range = 2.0f;
	pntLight1.Position = XMFLOAT3(0.0f, 0.5f, -1.0f);
	pntLight1.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	pntLight1.Attenuation = XMFLOAT3(0.0f, 0.2f, 1.0f);

	Game::vsPerSceneData->pntLights[0] = pntLight1;
	deviceContext->UpdateSubresource(
		Game::vsPerSceneConstBuffer,
		0,
		NULL,
		Game::vsPerSceneData,
		0,
		0);

	Game::vsPerFrameData->time		   = 0;

	return true;
}

void Gameplay::LoadShadersAndInputLayout()
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

	// Water Shaders -----------------------------------
	D3DReadFileToBlob(L"Shaders/VS_Water.cso", &vsBlob);
	D3DReadFileToBlob(L"Shaders/PS_Water.cso", &psBlob);
	// "I Have no idea what the fuck."
	HR(device->CreateInputLayout(
		vertex_Water_Desc,
		ARRAYSIZE(vertex_Water_Desc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	ResourceManager::AddInputLayout("Water", inputLayout);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("Water", vertexShader);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));
	ResourceManager::AddPixelShader("Water", pixelShader);

	ReleaseMacro(vsBlob);
	ReleaseMacro(psBlob);

	// Skybox Shaders -----------------------------------
	D3DReadFileToBlob(L"Shaders/VS_Skybox.cso", &vsBlob);
	D3DReadFileToBlob(L"Shaders/PS_Skybox.cso", &psBlob);

	HR(device->CreateInputLayout(
		vertex_Skybox_Desc,
		ARRAYSIZE(vertex_Skybox_Desc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));
	ResourceManager::AddInputLayout("skybox", inputLayout);

	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));
	ResourceManager::AddVertexShader("skybox", vertexShader);

	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader))
		ResourceManager::AddPixelShader("skybox", pixelShader);
}

void Gameplay::CreateGeometryBuffers()
{
	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC vsPerFrameBufferDesc;
	vsPerFrameBufferDesc.ByteWidth = sizeof(*Game::vsPerFrameData);
	vsPerFrameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerFrameBufferDesc.CPUAccessFlags = 0;
	vsPerFrameBufferDesc.MiscFlags = 0;
	vsPerFrameBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerFrameBufferDesc,
		NULL,
		&Game::vsPerFrameConstBuffer));

	D3D11_BUFFER_DESC vsPerModelBufferDesc;
	vsPerModelBufferDesc.ByteWidth = sizeof(*Game::vsPerModelData);
	vsPerModelBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerModelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerModelBufferDesc.CPUAccessFlags = 0;
	vsPerModelBufferDesc.MiscFlags = 0;
	vsPerModelBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerModelBufferDesc,
		NULL,
		&Game::vsPerModelConstBuffer));

	D3D11_BUFFER_DESC vsPerSceneBufferDesc;
	vsPerSceneBufferDesc.ByteWidth = sizeof(*Game::vsPerSceneData);
	vsPerSceneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vsPerSceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vsPerSceneBufferDesc.CPUAccessFlags = 0;
	vsPerSceneBufferDesc.MiscFlags = 0;
	vsPerSceneBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&vsPerSceneBufferDesc,
		NULL,
		&Game::vsPerSceneConstBuffer));
}

void Gameplay::LoadResources()
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

	HR(CreateWICTextureFromFile(
		device,
		deviceContext,
		L"Resources/water.jpg",
		0,
		&srv));
	ResourceManager::AddSRV("water",srv);

	// Meshes -------------------------------------------
	Mesh* cube = Mesh::LoadFromOBJ("Resources/boat_obj.obj");
	cube->Initialize(device, ResourceManager::GetInputLayout(cube->ILName()));

	Mesh* quad = Mesh::LoadFromOBJ("Resources/water_obj.obj");
	quad->Initialize(device, ResourceManager::GetInputLayout("Water"));

	Mesh* sphere = Mesh::LoadFromOBJ("Resources/cannonball_obj.obj");
	sphere->Initialize(device, ResourceManager::GetInputLayout(quad->ILName()));

	// Begin Disgusting.
	Vertex_PNC temp[] = 
	{
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,2), XMFLOAT3(0,0,0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,2,0), XMFLOAT3(0,0,0), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT4(1,0,0,1) },
		{ XMFLOAT3(2,0,0), XMFLOAT3(0,0,0), XMFLOAT4(1,0,0,1) }
	};

	Vertex_PNC* verts = new Vertex_PNC[6];

	for (int i = 0; i < 6; i++)
	{ verts[i] = temp[i]; }
	
	VertexArray vertArray = VertexArray();
	vertArray.data = verts;
	vertArray.IndividualBytes = sizeof(Vertex_PNC);
	vertArray.TotalArrayBytes = 6 * vertArray.IndividualBytes;
	vertArray.InputLayoutName = "PNC";

	UINT32 itemp[] = { 0,1,2,3,4,5 };
	UINT32* indices = new UINT[6];
	for (int i = 0; i < 6; i++)
	{ indices[i] = itemp[i]; }
	// End Disgusting.

	Mesh* coordinates = new Mesh(vertArray, 6, indices, 6);
	coordinates->Initialize(device, ResourceManager::GetInputLayout("PNC"));

	ResourceManager::AddMesh("coordinates", coordinates);
	ResourceManager::AddMesh("cube", cube);
	ResourceManager::AddMesh("quad", quad);
	ResourceManager::AddMesh("sphere", sphere);

	// Materials -----------------------------------------
	Material* crateMat = new Material(ResourceManager::GetSRV("crate"), ResourceManager::GetSamplerState("crate"));
	crateMat->Initialize(ResourceManager::GetVertexShader(cube->ILName()), ResourceManager::GetPixelShader(cube->ILName()));

	Material* waterMat = new Material(ResourceManager::GetSRV("water"), ResourceManager::GetSamplerState("crate"));
	waterMat->Initialize(ResourceManager::GetVertexShader("Water"), ResourceManager::GetPixelShader("Water"));

	Material* coordinatesMat = new Material(nullptr, nullptr);
	coordinatesMat->Initialize(ResourceManager::GetVertexShader(coordinates->ILName()), ResourceManager::GetPixelShader(coordinates->ILName()));

	ResourceManager::AddMaterial("crate", crateMat);
	ResourceManager::AddMaterial("water", waterMat);
	ResourceManager::AddMaterial("coordinates", coordinatesMat);
}

void Gameplay::SetupAudio()
{
		// grab audio device
	audioDevice = alcOpenDevice(NULL);
#if defined(DEBUG) | defined(_DEBUG)
	assert(audioDevice != NULL);
#endif

	// create context for device
	audioDeviceContext = alcCreateContext(audioDevice, NULL);
#if defined(DEBUG) | defined(_DEBUG)
	assert(audioDeviceContext != NULL);
#endif

	// assign context to 'current'
	alcMakeContextCurrent(audioDeviceContext);

	//float listenerOrientation[] = {										// UNCOMMENT THIS
	//	XMVectorGetX(CameraManager::ActiveCamera()->forward),
	//	XMVectorGetY(CameraManager::ActiveCamera()->forward),
	//	XMVectorGetZ(CameraManager::ActiveCamera()->forward),
	//	XMVectorGetX(CameraManager::ActiveCamera()->up),
	//	XMVectorGetY(CameraManager::ActiveCamera()->up),
	//	XMVectorGetZ(CameraManager::ActiveCamera()->up)
	//};
	//
	//// listener setup! :D
	//alListener3f(AL_POSITION, XMVectorGetX(CameraManager::ActiveCamera()->position), 
	//						  XMVectorGetY(CameraManager::ActiveCamera()->position), 
	//						  XMVectorGetZ(CameraManager::ActiveCamera()->position));
	//alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	//alListenerfv(AL_ORIENTATION, listenerOrientation);

#if defined(DEBUG) | defined(_DEBUG)
	std::cout << "listener position:" << "[" << XMVectorGetX(CameraManager::ActiveCamera()->position) 
									  << ", " << XMVectorGetY(CameraManager::ActiveCamera()->position) 
									  << ", " << XMVectorGetZ(CameraManager::ActiveCamera()->position) << "]" << std::endl;
#endif

	// ^ using assert to replace the following:
	/*audioDevice = alcOpenDevice(NULL);

#ifdef defined(DEBUG)
	if(audioDevice == NULL)
		std::cout << "cannot open sound card!" << std::endl;
#endif

	// device accessed successfully
	else
	{
		// create context for device
		audioDeviceContext = alcCreateContext(audioDevice, NULL);

#ifdef defined(DEBUG)
		if(audioDeviceContext == NULL)
			std::cout << "cannot create context for our device!" << std::endl;
#endif

		// context created successfully
		else
			alcMakeContextCurrent(audioDeviceContext); // assign our created context to 'current'
	}*/
}

void Gameplay::Update(float dt)
{
	CameraManager::Update();

	XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, CameraManager::ActiveCamera()->position);
	
	Game::vsPerFrameData->time		+= dt / 5.0f;
	Game::vsPerFrameData->view		 = CameraManager::ActiveCamera()->GetViewMatrix();
	Game::vsPerFrameData->projection = CameraManager::ActiveCamera()->GetProjMatrix();
	Game::vsPerFrameData->worldPosition = cameraPosition;

	if (Game::vsPerFrameData->time > 1.0f)
		Game::vsPerFrameData->time -= 1.0f;

	deviceContext->UpdateSubresource(
		Game::vsPerFrameConstBuffer,
		0,
		NULL,
		Game::vsPerFrameData,
		0,
		0);

	// Toggle drawing entities coordinates in debug.
	if (Input::KeyUp('I'))
		drawCoordinates = !drawCoordinates;

	// Toggle showing the cursor (not locked yet).
	if (Input::KeyUp('M'))
	{
		Input::ToggleCursorVisibility();
		Input::ToggleCursorLocking();
	}

	if (Input::KeyUp(' '))
	{
		// Boat 1 fires at Boat 2
		((Boat*)(entities[0]))->Fire(((Boat*)(entities[1])));

		// cannonball
		dynamic_cast<CannonBall*>(entities[3])->Fire(entities[0]->position, 
			(entities[0]->Right + entities[0]->Up));

		// text updates
		std::cout << "Boat 1 fire() called!" << std::endl;
		std::cout << "Boat 1 ammo left: " << ((Boat*)(entities[0]))->Ammunition() << std::endl;
		std::cout << "Boat 2 hp left: " << ((Boat*)(entities[1]))->Health() << std::endl;
	}
	if (Input::KeyUp('G'))
	{
		main_bgm->start();
	}

#if defined(SOUND_PLAY)
	// change velocity of audio
	if (Input::KeyUp('Q'))
	{
		main_bgm->changeVelocity(-0.0001f);
	}
	if (Input::KeyUp('W'))
	{
		main_bgm->changeVelocity(+0.0001f);
	}
#endif

	world->Update(dt, XMMatrixIdentity());

#ifdef SOUND_PLAY
	// updating audio source based on listener position
	float listenerPos[3];
	alGetListenerfv(AL_POSITION, listenerPos);
	main_bgm->updateGain(listenerPos);
	main_bgm->update();
#endif
}

void Gameplay::Draw(float dt)
{
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the buffer
	deviceContext->ClearRenderTargetView(DX::renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		DX::depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	deviceContext->VSSetConstantBuffers(0, 1, &Game::vsPerFrameConstBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &Game::vsPerModelConstBuffer);
	deviceContext->VSSetConstantBuffers(2, 1, &Game::vsPerSceneConstBuffer);

	for (std::vector<DrawableEntity*>::iterator it = entities.begin(); it != entities.end(); it++)
	{
		(*it)->Render(deviceContext);
	}
	
#if defined(DEBUG) | defined(_DEBUG)
	if (drawCoordinates)
	{
		ResourceManager::GetMaterial("coordinates")->SetShaders(deviceContext);
		Mesh* coords = ResourceManager::GetMesh("coordinates");
		coords->SetBuffers(deviceContext);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		for (std::vector<DrawableEntity*>::iterator it = entities.begin(); it != entities.end(); it++)
		{ 
			(*it)->SetConstantBuffer(deviceContext);

			deviceContext->DrawIndexed(
				coords->Indices(),
				0,
				0);
		}
	}
#endif

	// Present the buffer
	HR(Game::swapChain->Present(0, 0));
}