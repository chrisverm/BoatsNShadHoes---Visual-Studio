#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include <DirectXMath.h>
#include "Entity.h"

using namespace DirectX;

enum CameraMountState
{
	STATIC,
	ATTACHED,
	FIRST_PERSON,
	THIRD_PERSON,
};

struct CAMERA_DESC
{
	float FieldOfView;
	float NearPlane;
	float FarPlane;
	float AttachedDist;
	float* InitialRoll;
	XMVECTOR* InitialPosition;
	XMVECTOR* InitialForward;
	CameraMountState Position;
	CameraMountState Forward;
	CameraMountState Roll;

	CAMERA_DESC() { AttachedDist = 0; }
	~CAMERA_DESC() {}
};

class Camera
{
public:
	float fieldOfView;
	float aspectRatio;
	float nearPlane, farPlane;
	float attachedDist;
	float* roll;
	XMVECTOR* position;
	XMVECTOR* forward;
	XMVECTOR* right;
	XMVECTOR* up;

	Camera(CAMERA_DESC* cDesc);
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
	CameraMountState positionState;
	CameraMountState forwardState;
	CameraMountState rollState;
	XMFLOAT4X4 projMatrix;
	XMFLOAT4X4 viewMatrix;

};

class CameraQuick : public Entity
{
public:
	CameraQuick();
	~CameraQuick();

	float fieldOfView;
	float aspectRatio;
	float nearPlane, farPlane;

	void ResizeAspectRatio(float ratio);

	void Update(float dt);

	void SetProjMatrix();
	
	XMFLOAT4X4 GetProjMatrix() const { return projMatrix; }
	XMFLOAT4X4 GetViewMatrix() const { return viewMatrix; }
private:
	
	XMFLOAT4X4 projMatrix;
	XMFLOAT4X4 viewMatrix;
};

#endif