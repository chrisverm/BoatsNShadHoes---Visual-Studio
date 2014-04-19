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
	Entity* Parent;
	float InitialRoll;
	XMVECTOR InitialPosition;
	XMVECTOR InitialForward;
	CameraMountState Position;
	CameraMountState Forward;
	CameraMountState Roll;
};

class Camera : public Entity
{
public:
	float fieldOfView;
	float aspectRatio;
	float nearPlane, farPlane;
	float attachedDist;

	Camera(CAMERA_DESC* cDesc);
	~Camera();
	void Update(float dt, const XMMATRIX& parentMat);
	void ResizeAspectRatio(float ratio);
	void LookAt(XMVECTOR focus);
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

#endif