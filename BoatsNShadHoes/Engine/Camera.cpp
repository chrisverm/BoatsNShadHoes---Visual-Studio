#include "Camera.h"
#include <iostream>

Camera::Camera(CAMERA_DESC* cDesc)
	: Entity()
{
	fieldOfView = cDesc->FieldOfView;
	nearPlane = cDesc->NearPlane;
	farPlane = cDesc->FarPlane;

	attachedDist = cDesc->AttachedDist;

	rotation = XMVectorSetZ(rotation, cDesc->InitialRoll);
	rollState = cDesc->Roll;

	position = cDesc->InitialPosition;
	positionState = cDesc->Position;

	forward = cDesc->InitialForward;
	forwardState = cDesc->Forward;

	minAngleClamps = cDesc->MinAngleClamps;
	maxAngleClamps = cDesc->MaxAngleClamps;

	projMatrix = XMFLOAT4X4();
	viewMatrix = XMFLOAT4X4();

	cDesc->Parent->AddChild(this);
}

Camera::~Camera() { }

void Camera::Update(float dt, const XMMATRIX& parentMat)
{
	if (positionState != STATIC)
	{
		if (Input::DeltaMouse->x != 0 || Input::DeltaMouse->y != 0)
			Move(Input::DeltaMouse, dt);

		XMMATRIX trans = XMMatrixTranslationFromVector(XMVectorSetY(parent->position, XMVectorGetY(parent->position / 1.25f)));
		XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(XMVectorSetZ(parent->rotation, 0));
		XMMATRIX sca = XMMatrixScalingFromVector(parent->scale);
		XMMATRIX parentMatrix = sca * rot * trans;

		Entity::Update(dt, parentMatrix);
	}
	else
		Entity::Update(dt, XMMatrixIdentity());

	rotation = XMVectorClamp(rotation, minAngleClamps, maxAngleClamps);
	rotation = XMVectorModAngles(rotation);

	XMStoreFloat4x4(&viewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix)));
}

void Camera::Move(const XMINT2* mouseDelta, float dt)
{
	rotation = XMVectorSetX(rotation, XMVectorGetX(rotation) 
		+ XMConvertToRadians((float)mouseDelta->y) * dt * 50);
	rotation = XMVectorSetY(rotation, XMVectorGetY(rotation) 
		+ XMConvertToRadians((float)mouseDelta->x) * dt * 50);

	rotation = XMVectorClamp(rotation, minAngleClamps, maxAngleClamps);
	rotation = XMVectorModAngles(rotation);

	UpdateForwardFromRotation();

	if (forwardState == THIRD_PERSON)
	{
		position = XMVectorSetX(position, 0 - attachedDist * XMVectorGetX(forward));
		position = XMVectorSetY(position, 0 - attachedDist * XMVectorGetY(forward));
		position = XMVectorSetZ(position, 0 - attachedDist * XMVectorGetZ(forward));
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