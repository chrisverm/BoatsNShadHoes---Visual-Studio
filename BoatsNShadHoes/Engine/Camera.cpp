#include "Camera.h"

Camera::Camera(CAMERA_DESC* cDesc)
	: Entity(false)
{
	fieldOfView = cDesc->FieldOfView;
	nearPlane = cDesc->NearPlane;
	farPlane = cDesc->FarPlane;

	attachedDist = cDesc->AttachedDist;

	roll = cDesc->InitialRoll;
	rollState = cDesc->Roll;

	position = cDesc->InitialPosition;
	positionState = cDesc->Position;

	forward = cDesc->InitialForward;
	forwardState = cDesc->Forward;

	projMatrix = XMFLOAT4X4();
	viewMatrix = XMFLOAT4X4();

	cDesc->Parent->AddChild(this);
}

Camera::~Camera() { }

void Camera::Update(float dt, const XMMATRIX& parentMat)
{
	Entity::Update(dt, parentMat);
	XMStoreFloat4x4(&viewMatrix, XMMatrixInverse(nullptr,(XMLoadFloat4x4(&worldMatrix))));

	// Mount stuff here
	switch (positionState)
	{
	case STATIC:
		// do nothing?
		break;
	case ATTACHED:
		SetViewMatrix();		// only if view changed
		break;
	case FIRST_PERSON:
	case THIRD_PERSON:

		break;
	}

	switch (forwardState)
	{
	case STATIC:
		// do nothing?
		break;
	case ATTACHED:

		break;
	case FIRST_PERSON:
	case THIRD_PERSON:

		break;
	}

	switch (rollState)
	{
	case STATIC:
		// do nothing?
		break;
	case ATTACHED:

		break;
	case FIRST_PERSON:
	case THIRD_PERSON:

		break;
	}
}

void Camera::ResizeAspectRatio(float ratio)
{
	aspectRatio = ratio;
}

void Camera::LookAt(XMVECTOR focus)
{
	forward = XMVector3Normalize(focus - position);
}

void Camera::SetProjMatrix()
{
	assert(aspectRatio != 0);
	assert(farPlane != nearPlane);

	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(
		fieldOfView, aspectRatio, nearPlane, farPlane)));
}

void Camera::SetViewMatrix()
{
	SetUnitVectors();

	XMVECTOR eye = position - forward * attachedDist;
	XMVECTOR focus = eye + forward;

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(
		XMMatrixLookAtLH(eye, focus, up)));
}