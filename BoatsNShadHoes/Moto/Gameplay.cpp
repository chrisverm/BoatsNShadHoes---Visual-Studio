#include "Gameplay.h"

Gameplay::Gameplay(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameState(device, deviceContext) { }

Gameplay::~Gameplay()
{
#ifdef SOUND_PLAY
	alcDestroyContext(audioDeviceContext);
	alcCloseDevice(audioDevice);

	if (main_bgm != nullptr)
	{
		delete main_bgm;
		main_bgm = nullptr;
	}

	for (std::map<std::string, AudioPlayer*>::iterator it = sounds.begin(); it != sounds.end(); it = sounds.begin())
	{ delete it->second; sounds.erase(it->first); }
#endif

	// every entity is somehow connected to this
	// thus this will destroy all entities
	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}
}

bool Gameplay::Initialize()
{
	world = new Entity();

	LoadShadersAndInputLayout();
	LoadResources();

#if defined(DEBUG) | defined(_DEBUG)
	Bounds::mesh = Resources::GetMesh("cube");
	Bounds::mat = Resources::GetMaterial("crate");

	Entity::coordinateMesh = Resources::GetMesh("coordinates");
	Entity::coordinateMaterial = Resources::GetMaterial("coordinates");
#endif

	Input::SetCursorVisibility(false);
	Input::SetCursorLocking(true);

	// stats for first boat
	BOAT_STATS b1Stats;
	b1Stats.health			= 100.0f;
	b1Stats.armor			= 10.0f;
	b1Stats.ammunition		= 0;
	b1Stats.maxAmmunition	= 5;
	b1Stats.rateOfFire		= 0.7f;
	b1Stats.damage			= 20;

	// Entites -----------------------------------------
	playerBoat = new Boat(Resources::GetMesh("ship"), Resources::GetMaterial("ship"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"), b1Stats, true);
	playerBoat->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	// populate ammunition
	for (short i = 0; i < playerBoat->MaximumAmmunition(); i++)
	{
		CannonBall* cannonBall = new CannonBall(Resources::GetMesh("sphere"), Resources::GetMaterial("cannonball"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"));

		cannonBall->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
		cannonBall->position = XMVectorSet(0,-10,0,0);

		// try to add ammunition
		if(playerBoat->AddAmmunition(cannonBall))
		{
			std::cout << "added ammo!" << std::endl;
			world->AddChild(cannonBall); // does not work well when attached to boat!
		}

		// delete created pointer in case we couldn't add ammo
		else
		{
			std::cout << " could not add ammo!" << std::endl;
			delete cannonBall;
		}
	}

	//boat->SetStats(b1Stats);

	// stats for second boat
	BOAT_STATS b2Stats;
	b2Stats.health			= 50.0f;
	b2Stats.armor			= 15.0f;
	b2Stats.ammunition		= 0;
	b2Stats.maxAmmunition	= 3;
	b2Stats.rateOfFire		= 1.5f;
	b2Stats.damage			= 10;

	otherBoat = new Boat(Resources::GetMesh("ship"), Resources::GetMaterial("ship"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"), b2Stats, false);
	otherBoat->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
	otherBoat->SetPosition(5, 0, 15);
	otherBoat->SetRotation(0, 2, 0);

	// populate ammunition
	for(short i = 0; i < otherBoat->MaximumAmmunition(); i++)
	{
		CannonBall* cannonBall = new CannonBall(Resources::GetMesh("sphere"), Resources::GetMaterial("cannonball"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"));

		cannonBall->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
		cannonBall->position = XMVectorSet(0,-10,0,0);

		// try to add ammunition
		if(otherBoat->AddAmmunition(cannonBall))
		{
			std::cout << "added ammo!" << std::endl;
			world->AddChild(cannonBall);
		}

		// delete created pointer in case we couldn't add ammo
		else
		{
			std::cout << " could not add ammo!" << std::endl;
			delete cannonBall;
		}
	}

	//boat2->SetStats(b2Stats);

	/*CannonBall* cannonBall = new CannonBall(Resources::GetMesh("sphere"), Resources::GetMaterial("cannonball"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"));
	cannonBall->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);
	cannonBall->position = XMVectorSet(0,-10,0,0);*/

	SkyBox* skyBall = new SkyBox(Resources::GetMesh("skybox"), Resources::GetMaterial("skybox"), 
		Resources::GetRasterizerState("skybox"), Resources::GetDepthStencilState("skybox"));
	skyBall->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	Water* water = new Water(Resources::GetMesh("quad"), Resources::GetMaterial("water"), 
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"));
	water->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	world->AddChild(playerBoat);
	world->AddChild(otherBoat);
	world->AddChild(water);
	//entities.push_back(cannonBall);	world->AddChild(cannonBall);
	playerBoat->AddChild(skyBall); // Parented to the boat, AW YEAH.

	// Camera Setup -----------[ o]---------------------
	viewChanged = false;
	CameraManager::Initialize(deviceContext, 1, &DX::windowWidth, &DX::windowHeight, &viewChanged, &Game::projChanged);
	
	CAMERA_DESC camDesc;
	camDesc.FieldOfView = XMConvertToRadians(45.0f);
	camDesc.NearPlane = 0.1f;
	camDesc.FarPlane = 100.0f;
	camDesc.AttachedDist = 20.0f;
	camDesc.Parent = playerBoat;
	camDesc.InitialRoll = 0.0f;
	camDesc.InitialPosition = XMVectorSet(0.0f, 7.0f, -15.0f, 0.0f);
	camDesc.InitialForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camDesc.MinAngleClamps = XMVectorSet(0.0f, -181.0f, 0.0f, 0.0f) * XMConvertToRadians(1.0f);
	camDesc.MaxAngleClamps = XMVectorSet(90.0f, 181.0f, 0.0f, 0.0f) * XMConvertToRadians(1.0f);
	camDesc.Position = STATIC;
	camDesc.Forward = THIRD_PERSON;
	camDesc.Roll = STATIC;
	CameraManager::CreateNewCamera(&camDesc, true);

	

#ifdef SOUND_PLAY
	// AL setup --------------------------------~^^~-----
	SetupAudio();

	// Audio ---------------------d[-_-]b-----------------
	//main_bgm = new AudioPlayer("Resources/Zedd-Clarity_BGM.ogg");

	Boat::hitSounds.push_back(sounds["Hit1"] = new AudioPlayer("Resources/Hit1.wav"));
	Boat::hitSounds.push_back(sounds["Hit2"] = new AudioPlayer("Resources/Hit2.wav"));
	Boat::hitSounds.push_back(sounds["Hit3"] = new AudioPlayer("Resources/Hit3.wav"));
	Boat::hitSounds.push_back(sounds["Hit4"] = new AudioPlayer("Resources/Hit4.wav"));
	Boat::hitSounds.push_back(sounds["Hit5"] = new AudioPlayer("Resources/Hit5.wav"));
	Boat::hitSounds.push_back(sounds["Hit6"] = new AudioPlayer("Resources/Hit6.wav"));
	Boat::hitSounds.push_back(sounds["Hit7"] = new AudioPlayer("Resources/Hit7.wav"));
	Boat::hitSounds.push_back(sounds["Hit8"] = new AudioPlayer("Resources/Hit8.wav"));
	Boat::hitSounds.push_back(sounds["Hit9"] = new AudioPlayer("Resources/Hit9.wav"));
	
	Boat::fireSounds.push_back(sounds["Fire1"] = new AudioPlayer("Resources/CannonFire1.wav"));
	Boat::fireSounds.push_back(sounds["Fire2"] = new AudioPlayer("Resources/CannonFire2.wav"));
	Boat::fireSounds.push_back(sounds["Fire3"] = new AudioPlayer("Resources/CannonFire3.wav"));

	CannonBall::splashes.push_back(sounds["Splash1"] = new AudioPlayer("Resources/Splash1.wav"));
	CannonBall::splashes.push_back(sounds["Splash2"] = new AudioPlayer("Resources/Splash2.wav"));
	CannonBall::splashes.push_back(sounds["Splash3"] = new AudioPlayer("Resources/Splash1.wav"));
	CannonBall::splashes.push_back(sounds["Splash4"] = new AudioPlayer("Resources/Splash2.wav"));
	CannonBall::splashes.push_back(sounds["Splash5"] = new AudioPlayer("Resources/Splash1.wav"));
	CannonBall::splashes.push_back(sounds["Splash6"] = new AudioPlayer("Resources/Splash1.wav"));

	for (std::map<std::string, AudioPlayer*>::iterator it = sounds.begin(); it != sounds.end(); it++)
	{ it->second->init(); }

	//main_bgm->init();
	//main_bgm->generateBufferData();
	//main_bgm->setLooping(AL_TRUE);
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

	Game::vsPerFrameData->time = 0;

	return true;
}

void Gameplay::LoadShadersAndInputLayout()
{
	// PNU Shaders -------------------------------------
	Resources::CreateVertexShaderAndInputLayout("PNU", L"Shaders/VS_PNU.cso", 
		vertex_PNU_Desc, ARRAYSIZE(vertex_PNU_Desc));
	Resources::CreatePixelShader("PNU", L"Shaders/PS_PNU.cso");

	// PNC Shaders -------------------------------------
	Resources::CreateVertexShaderAndInputLayout("PNC", L"Shaders/VS_PNC.cso", 
		vertex_PNC_Desc, ARRAYSIZE(vertex_PNC_Desc));
	Resources::CreatePixelShader("PNC", L"Shaders/PS_PNC.cso");

	// PNUC Shaders -------------------------------------
	/*Resources::CreateVertexShaderAndInputLayout("PNUC", L"Shaders/VS_PNUC.cso", 
		vertex_PNUC_Desc, ARRAYSIZE(vertex_PNUC_Desc));
	Resources::CreatePixelShader("PNUC", L"Shaders/PS_PNUC.cso");*/

	// Water Shaders -----------------------------------
	Resources::CreateVertexShaderAndInputLayout("Water", L"Shaders/VS_Water.cso", 
		vertex_Water_Desc, ARRAYSIZE(vertex_Water_Desc));
	Resources::CreatePixelShader("Water", L"Shaders/PS_Water.cso");

	// Skybox Shaders -----------------------------------
	Resources::CreateVertexShaderAndInputLayout("skybox", L"Shaders/VS_Skybox.cso", 
		vertex_Skybox_Desc, ARRAYSIZE(vertex_Skybox_Desc));
	Resources::CreatePixelShader("skybox", L"Shaders/PS_Skybox.cso");
}

void Gameplay::LoadResources()
{
	// Meshes -------------------------------------------
	Resources::CreateMesh("ship", "Resources/PirateShip_obj.obj");
	//Resources::CreateMesh("quad", "Resources/water_obj.obj", Resources::GetInputLayout("Water"));
	Resources::CreateMesh("sphere", "Resources/cannonball_obj.obj");
	Resources::CreateMesh("skybox", "Resources/cannonball_obj.obj", Resources::GetInputLayout("skybox"));
	Resources::CreateMesh("cube", "Resources/crate_obj.obj");

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

	Mesh* coordinates = new Mesh(vertArray, 6, indices, 6, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	coordinates->Initialize(device, Resources::GetInputLayout("PNC"));

	Resources::AddMesh("coordinates", coordinates);

	// And lo, on this such line there was more disgusting.
	const int DIM = 40;
	Vertex_PNU* waterTemp = new Vertex_PNU[DIM * DIM * 6];
	int index = 0;
	XMFLOAT3 offset = XMFLOAT3(DIM / 2.0f, 0, DIM / 2.0f);
	float div = DIM; // prevents width/depth from being greater than 1
	for (float row = 0; row < DIM; row++)
	{
		for (float col = 0; col < DIM; col++)
		{
			/* first triangle */
			waterTemp[index].Position	= XMFLOAT3((col - offset.x) / div, 0.0f, (row - offset.z) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(0.0f, 0.0f);
			index++;

			waterTemp[index].Position	= XMFLOAT3((col - offset.x + 1) / div, 0.0f, (row - offset.z) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(1.0f, 0.0f);
			index++;

			waterTemp[index].Position	= XMFLOAT3((col - offset.x + 1) / div, 0.0f, (row - offset.z + 1) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(1.0f, 1.0f);
			index++;

			/* second triangle */
			waterTemp[index].Position	= XMFLOAT3((col - offset.x) / div, 0.0f, (row - offset.z) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(0.0f, 0.0f);
			index++;

			waterTemp[index].Position	= XMFLOAT3((col - offset.x + 1) / div, 0.0f, (row - offset.z + 1) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(1.0f, 1.0f);
			index++;

			waterTemp[index].Position	= XMFLOAT3((col - offset.x) / div, 0.0f, (row - offset.z + 1) / div);
			waterTemp[index].Normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			waterTemp[index].UV			= XMFLOAT2(0.0f, 1.0f);
			index++;
		}
	}
	UINT* indices2 = new UINT[DIM * DIM * 6];
	for (int i = 0; i < DIM * DIM * 6; i++)
	{ indices2[i] = i; }

	VertexArray va;
	va.data = waterTemp;
	va.IndividualBytes = sizeof(Vertex_PNU);
	va.TotalArrayBytes = sizeof(Vertex_PNU) * (DIM * DIM * 6);
	va.InputLayoutName = "PNU";

	Mesh* m = new Mesh(va, DIM * DIM * 6, indices2, DIM * DIM * 6, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m->Initialize(device, Resources::GetInputLayout(m->ILName()));
	Resources::AddMesh("quad", m);

	// And lo, on this such line it did end.

	// Shader Resource Views -----------------------------
	Resources::CreateShaderResourceView("crate", L"Resources/crate_texture.png");
	Resources::CreateShaderResourceView("water", L"Resources/water_texture.jpg");
	Resources::CreateShaderResourceView("cannonball", L"Resources/cannonball_texture.jpg");
	Resources::CreateShaderResourceView("skybox", L"Resources/skybox_environment.dds");
	Resources::CreateShaderResourceView("ship", L"Resources/boat_texture.png");

	// Sampler States ------------------------------------
	D3D11_SAMPLER_DESC samplerDesc;
	
	// MIN_MAG_POINT_MIP_LINEAR
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Resources::CreateSamplerState("MIN_MAG_POINT_MIP_LINEAR", samplerDesc);

	// MIN_MAP_MIP_LINEAR (Trilinear)
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter	 = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Resources::CreateSamplerState("MIN_MAG_MIP_LINEAR", samplerDesc);

	// Materials -----------------------------------------
	Resources::CreateMaterial("crate", Resources::GetSRV("crate"), Resources::GetSamplerState("MIN_MAG_POINT_MIP_LINEAR"),
		Resources::GetVertexShader(Resources::GetMesh("ship")->ILName()), Resources::GetPixelShader(Resources::GetMesh("ship")->ILName()));

	Resources::CreateMaterial("water", Resources::GetSRV("water"), Resources::GetSamplerState("MIN_MAG_POINT_MIP_LINEAR"),
		Resources::GetVertexShader("Water"), Resources::GetPixelShader("Water"));

	Resources::CreateMaterial("cannonball", Resources::GetSRV("cannonball"), Resources::GetSamplerState("MIN_MAG_POINT_MIP_LINEAR"),
		Resources::GetVertexShader(Resources::GetMesh("sphere")->ILName()), Resources::GetPixelShader(Resources::GetMesh("sphere")->ILName()));

	Resources::CreateMaterial("coordinates", nullptr, nullptr, Resources::GetVertexShader(coordinates->ILName()), Resources::GetPixelShader(coordinates->ILName()));

	Resources::CreateMaterial("skybox", Resources::GetSRV("skybox"), Resources::GetSamplerState("MIN_MAG_MIP_LINEAR"),
		Resources::GetVertexShader("skybox"), Resources::GetPixelShader("skybox"));

	Resources::CreateMaterial("ship", Resources::GetSRV("ship"), Resources::GetSamplerState("MIN_MAG_POINT_MIP_LINEAR"), 
		Resources::GetVertexShader(Resources::GetMesh("ship")->ILName()), Resources::GetPixelShader(Resources::GetMesh("ship")->ILName()));

	// Depth Stencil States ----------------------------
	D3D11_DEPTH_STENCIL_DESC dssDesc;

	// Default
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable					= true;
	dssDesc.DepthWriteMask				= D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc					= D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable				= false;
	dssDesc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	dssDesc.StencilWriteMask			= D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dssDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilDepthFailOp= D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	Resources::CreateDepthStencilState("default", dssDesc);

	// Skybox
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable					= true;
	dssDesc.DepthWriteMask				= D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc					= D3D11_COMPARISON_LESS_EQUAL;
	dssDesc.StencilEnable				= false;
	dssDesc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
	dssDesc.StencilWriteMask			= D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilDepthFailOp= D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	Resources::CreateDepthStencilState("skybox", dssDesc);

	// Rasterizer States -------------------------------
	D3D11_RASTERIZER_DESC rsDesc;

	// Default
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode					= D3D11_FILL_SOLID;
	rsDesc.CullMode					= D3D11_CULL_NONE; // the face to "cull" - not show
	rsDesc.FrontCounterClockwise	= false;
	rsDesc.DepthBias				= 0;
	rsDesc.DepthBiasClamp			= 0.0f;
	rsDesc.SlopeScaledDepthBias		= 0.0f;
	rsDesc.DepthClipEnable			= true;
	rsDesc.ScissorEnable			= false;
	rsDesc.MultisampleEnable		= true;
	rsDesc.AntialiasedLineEnable	= false;
	Resources::CreateRasterizerState("default", rsDesc);

	// Wireframe (TEST)
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode					= D3D11_FILL_WIREFRAME;
	rsDesc.CullMode					= D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise	= false;
	rsDesc.DepthBias				= 0;
	rsDesc.DepthBiasClamp			= 0.0f;
	rsDesc.SlopeScaledDepthBias		= 0.0f;
	rsDesc.DepthClipEnable			= true;
	rsDesc.ScissorEnable			= false;
	rsDesc.MultisampleEnable		= false;
	rsDesc.AntialiasedLineEnable	= false;
	Resources::CreateRasterizerState("wireframe", rsDesc);

	// Skybox
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode					= D3D11_FILL_SOLID;
	rsDesc.CullMode					= D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise	= false;
	rsDesc.DepthBias				= 0;
	rsDesc.DepthBiasClamp			= 0.0f;
	rsDesc.SlopeScaledDepthBias		= 0.0f;
	rsDesc.DepthClipEnable			= false;
	rsDesc.ScissorEnable			= false;
	rsDesc.MultisampleEnable		= false;
	rsDesc.AntialiasedLineEnable	= false;
	Resources::CreateRasterizerState("skybox", rsDesc);
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
	Game::vsPerFrameData->cameraPosition = cameraPosition;

	if (Game::vsPerFrameData->time >= D3D11_FLOAT32_MAX)
		Game::vsPerFrameData->time -= D3D11_FLOAT32_MAX;

	deviceContext->UpdateSubresource(
		Game::vsPerFrameConstBuffer,
		0,
		NULL,
		Game::vsPerFrameData,
		0,
		0);

	// Toggle showing the cursor (not locked yet).
	if (Input::KeyUp('M'))
	{
		Input::ToggleCursorVisibility();
		Input::ToggleCursorLocking();
	}

	if (Input::KeyUp(' '))
	{
		// Boat 1 fires at Boat 2
		playerBoat->Fire(otherBoat);

		// text updates
		std::cout << "Boat 1 fire() called!" << std::endl;
	}
	
	world->Update(dt, XMMatrixIdentity());

	if (Bounds::Intersecting(playerBoat->bounds, otherBoat->bounds))
		printf("Boat Collision! \n");

#if defined(SOUND_PLAY)
	if (Input::KeyUp('G'))
	{
		//main_bgm->start();
	}
	// change velocity of audio
	if (Input::KeyUp('Q'))
	{
		//main_bgm->changeVelocity(-0.0001f);
	}
	if (Input::KeyUp('W'))
	{
		//main_bgm->changeVelocity(+0.0001f);
	}
#endif


#ifdef SOUND_PLAY
	// updating audio source based on listener position
	float listenerPos[3];
	listenerPos[0] = cameraPosition.x;
	listenerPos[1] = cameraPosition.y;
	listenerPos[2] = cameraPosition.z;
	alGetListenerfv(AL_POSITION, listenerPos);
	for (std::map<std::string, AudioPlayer*>::iterator it = sounds.begin(); it != sounds.end(); it++)
	{ it->second->updateGain(listenerPos);  }
	//main_bgm->updateGain(listenerPos);
	//main_bgm->update();
#endif

#if defined(DEBUG) | defined(_DEBUG)
	// Toggle drawing Bounding boxes (these are kinda approximate).
	if (Input::KeyUp('B'))
		Bounds::drawBounds = !Bounds::drawBounds;

	// Toggle drawing entities coordinates in debug.
	if (Input::KeyUp('I'))
		Entity::drawCoordinates = !Entity::drawCoordinates ;
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

	world->Render(deviceContext);
	
#if defined(DEBUG) | defined(_DEBUG)
	
	world->DebugRender(deviceContext);

#endif

	// Present the buffer
	HR(Game::swapChain->Present(0, 0));
}