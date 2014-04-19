#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include "Camera.h"

class CameraManager
{
public:
	static void Initialize(ID3D11DeviceContext* deviceContext, int numViewports,
		int* windowWidth, int* windowHeight, bool* viewChanged, bool* projChanged);
	static void CreateNewCamera(CAMERA_DESC* cDesc, bool makeActive = false);
	static void ChangeActiveCamera(int index);
	static void Update();
	static void Release();

	static bool IsEmpty() { return numCameras < 0; }
	static float AspectRatio() { return (float)*windowWidth / *windowHeight; }
	static const Camera* ActiveCamera() { return cameras[activeIndex]; } 

	static int* windowWidth;
	static int* windowHeight;
	static bool* viewChanged;
	static bool* projChanged;

	static int numCameras;
	static int activeIndex;
	static Camera** cameras;

};

#endif