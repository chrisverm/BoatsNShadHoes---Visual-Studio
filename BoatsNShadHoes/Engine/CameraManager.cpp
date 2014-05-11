#include "CameraManager.h"

int* CameraManager::windowWidth = nullptr;
int* CameraManager::windowHeight = nullptr;
bool* CameraManager::viewChanged = nullptr;
bool* CameraManager::projChanged = nullptr;

int CameraManager::numCameras = 0;
int CameraManager::activeIndex = -1;
Camera** CameraManager::cameras = nullptr;

void CameraManager::Initialize(ID3D11DeviceContext* deviceContext, int numViewports, 
							   int* windowWidth, int* windowHeight, bool* viewChanged, bool* projChanged)
{
	CameraManager::windowWidth = windowWidth;
	CameraManager::windowHeight = windowHeight;
	CameraManager::viewChanged = viewChanged;
	CameraManager::projChanged = projChanged;
}

void CameraManager::CreateNewCamera(CAMERA_DESC* cDesc, bool makeActive)
{
	Camera* camera = new Camera(cDesc);

	numCameras++;

	Camera** newCameraList = new Camera*[numCameras];
	for (int i = 0; i < numCameras - 1; i++)
	{
		newCameraList[i] = cameras[i];
	}
	newCameraList[numCameras - 1] = camera;

	delete cameras;
	cameras = new Camera*[numCameras];
	for (int i = 0; i < numCameras; i++)
	{
		cameras[i] = newCameraList[i];
	}

	delete[] newCameraList;
	newCameraList = nullptr;

	if (makeActive)
		ChangeActiveCamera(numCameras - 1);
}

void CameraManager::ChangeActiveCamera(int index)
{
	if (IsEmpty())
		return;

	activeIndex = index;
	
	cameras[activeIndex]->ResizeAspectRatio(AspectRatio());
	cameras[activeIndex]->SetProjMatrix();
	cameras[activeIndex]->SetViewMatrix();
}

void CameraManager::Update()
{
	if (*viewChanged)
	{
		cameras[activeIndex]->SetViewMatrix();
		*viewChanged = false;
	}

	if (*projChanged)
	{
		cameras[activeIndex]->ResizeAspectRatio(AspectRatio());
		cameras[activeIndex]->SetProjMatrix();
		*projChanged = false;
	}

	//if (!IsEmpty())
	//	cameras[activeIndex]->Update(0, XMMatrixIdentity()); // CHANGE THESE PARAMS // UNCOMMENT THIS
}

void CameraManager::Release()
{
	if (cameras != nullptr)
	{
		for (int i = 0; i < numCameras; i++)
		{
			delete cameras[i];
			cameras[i] = nullptr;
		}

		delete[] cameras;
		cameras = nullptr;
	}

	numCameras = 0;
	activeIndex = -1;
}