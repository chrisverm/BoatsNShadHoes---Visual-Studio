#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include <DirectXMath.h>
#include "Entity.h"

using namespace DirectX;

class Camera
{
public:
	struct Mount
	{
		enum State { STATIC, FIXED, FREE };

		State position;
		State direction;
		State rotation;
	};

	float fieldOfView;
	float aspectRatio;
	float nearPlane, farPlane;
	float roll;
	XMVECTOR* position;
	XMVECTOR* forward;
	XMVECTOR* right;
	XMVECTOR* up;

	float attachedDist;

	Camera(Mount mount, float fov = XMConvertToRadians(45.0f), float nearP = 0.1f, float farP = 100.0f);
	~Camera();
	void Update();
	void ResizeAspectRatio(float ratio);
	void LookAt(XMVECTOR focus);
	void AttachTo(Entity* entity, float attachedDist);
	void SetUnitVectors();
	void SetProjMatrix();
	void SetViewMatrix();

	XMFLOAT4X4 GetProjMatrix() const { return projMatrix; }
	XMFLOAT4X4 GetViewMatrix() const { return viewMatrix; }

private:
	Mount mount;
	XMFLOAT4X4 projMatrix;
	XMFLOAT4X4 viewMatrix;

};

#endif