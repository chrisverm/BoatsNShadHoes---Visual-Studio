#include "Instructions.h"

Instructions::Instructions(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameState(device, deviceContext) { }

Instructions::~Instructions() { Unload(); }

void Instructions::Unload()
{
	// every entity is somehow connected to this
	// thus this will destroy all entities
	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}

	GameState::Unload();
}

bool Instructions::Initialize()
{
	world = new Entity();
	drawCoordinates = false;

	LoadShadersAndInputLayout();
	LoadResources();

	Input::SetCursorVisibility(false);
	Input::SetCursorLocking(true);

	Screen* screen = new Screen(Resources::GetMesh("screen"), Resources::GetMaterial("screen"),
		Resources::GetRasterizerState("default"), Resources::GetDepthStencilState("default"));
	screen->Initialize(Game::vsPerModelConstBuffer, Game::vsPerModelData);

	entities.push_back(screen);		world->AddChild(screen);

	// Camera Setup -----------[ o]---------------------
	viewChanged = false;
	CameraManager::Initialize(deviceContext, 1, &DX::windowWidth, &DX::windowHeight, &viewChanged, &Game::projChanged);
	
	CAMERA_DESC camDesc;
	camDesc.FieldOfView = XMConvertToRadians(45.0f);
	camDesc.NearPlane = 0.1f;
	camDesc.FarPlane = 100.0f;
	camDesc.AttachedDist = 20.0f;
	camDesc.Parent = screen;
	camDesc.InitialRoll = 0.0f;
	camDesc.InitialPosition = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	camDesc.InitialForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camDesc.MinAngleClamps = XMVectorSet(0.0f, -181.0f, 0.0f, 0.0f) * XMConvertToRadians(1.0f);
	camDesc.MaxAngleClamps = XMVectorSet(90.0f, 181.0f, 0.0f, 0.0f) * XMConvertToRadians(1.0f);
	camDesc.Position = STATIC;
	camDesc.Forward = THIRD_PERSON;
	camDesc.Roll = STATIC;
	CameraManager::CreateNewCamera(&camDesc, true);

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

void Instructions::LoadShadersAndInputLayout()
{
	// PNU Shaders -------------------------------------
	/*Resources::CreateVertexShaderAndInputLayout("PNU", L"Shaders/VS_PNU.cso", 
		vertex_PNU_Desc, ARRAYSIZE(vertex_PNU_Desc));
	Resources::CreatePixelShader("PNU", L"Shaders/PS_PNU.cso");

	// PNC Shaders -------------------------------------
	Resources::CreateVertexShaderAndInputLayout("PNC", L"Shaders/VS_PNC.cso", 
		vertex_PNC_Desc, ARRAYSIZE(vertex_PNC_Desc));
	Resources::CreatePixelShader("PNC", L"Shaders/PS_PNC.cso");*/

	// PNUC Shaders -------------------------------------
	Resources::CreateVertexShaderAndInputLayout("PNUC", L"Shaders/VS_PNUC.cso", 
		vertex_PNUC_Desc, ARRAYSIZE(vertex_PNUC_Desc));
	Resources::CreatePixelShader("PNUC", L"Shaders/PS_PNUC.cso");
}

void Instructions::LoadResources()
{
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

	Resources::CreateShaderResourceView("screen", L"Resources/screen_texture.jpg");

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

	//Begin More Disgusting
	Vertex_PNUC temp2[] = 
	{
		{ XMFLOAT3(1,1,0), XMFLOAT3(0,0,0), XMFLOAT2(1, 0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(-1,1,0), XMFLOAT3(0,0,0), XMFLOAT2(0, 0), XMFLOAT4(0,1,0,1) },
		{ XMFLOAT3(-1,-1,0), XMFLOAT3(0,0,0), XMFLOAT2(0, 1), XMFLOAT4(0,0,1,1) },
		{ XMFLOAT3(1,-1,0), XMFLOAT3(0,0,0), XMFLOAT2(1, 1), XMFLOAT4(0,0,1,1) }
	};

	Vertex_PNUC* verts2 = new Vertex_PNUC[4];
	for (int i = 0; i < 4; i++)
	{ verts2[i] = temp2[i]; }

	VertexArray vertArray2 = VertexArray();
	vertArray2.data = verts2;
	vertArray2.IndividualBytes = sizeof(Vertex_PNUC);
	vertArray2.TotalArrayBytes = 4 * vertArray2.IndividualBytes;
	vertArray2.InputLayoutName = "PNUC";

	UINT32 itemp2[] = { 0,3,2,0,2,1 };
	UINT32* indices2 = new UINT[6];
	for (int i = 0; i < 6; i++)
	{ indices2[i] = itemp2[i]; }

	//End More Disgusting

	Mesh* screen = new Mesh(vertArray2, 4, indices2, 6, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	screen->Initialize(device, Resources::GetInputLayout("PNUC"));

	Resources::AddMesh("screen", screen);

	Resources::CreateMaterial("screen", Resources::GetSRV("screen"), Resources::GetSamplerState("MIN_MAG_POINT_MIP_LINEAR"),
		Resources::GetVertexShader("PNUC"), Resources::GetPixelShader("PNUC"));
}

void Instructions::Update(float dt)
{
	CameraManager::Update();

	XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, CameraManager::ActiveCamera()->position);
	
	//Game::vsPerFrameData->time		+= dt / 5.0f;
	Game::vsPerFrameData->view		 = CameraManager::ActiveCamera()->GetViewMatrix();
	Game::vsPerFrameData->projection = CameraManager::ActiveCamera()->GetProjMatrix();
	Game::vsPerFrameData->cameraPosition = cameraPosition;

	//entities[4]->position = CameraManager::ActiveCamera()->position;

	if (Game::vsPerFrameData->time > 1.0f)
		Game::vsPerFrameData->time -= 1.0f;

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

	world->Update(dt, XMMatrixIdentity());

	if (Input::KeyUp(' '))
	{
		GameStateManager::ChangeState("Gameplay");
	}
}

void Instructions::Draw(float dt)
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

	// Present the buffer
	HR(Game::swapChain->Present(0, 0));
}