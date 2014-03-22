#include "Camera.h"

Camera::Camera(CAMERA_DESC* cDesc)
{
	fieldOfView = cDesc->FieldOfView;
	nearPlane = cDesc->NearPlane;
	farPlane = cDesc->FarPlane;

	roll = cDesc->InitialRoll;
	rollState = cDesc->Roll;

	position = cDesc->InitialPosition;
	positionState = cDesc->Position;

	forward = cDesc->InitialForward;
	forwardState = cDesc->Forward;

	right = new XMVECTOR();
	up = new XMVECTOR();

	attachedDist = 0.0f;

	projMatrix = XMFLOAT4X4();
	viewMatrix = XMFLOAT4X4();
}

Camera::~Camera()
{
	if (positionState != ATTACHED)
		delete position;

	if (forwardState != ATTACHED)
		delete forward;

	if (rollState != ATTACHED)
		delete roll;

	delete right;
	delete up;
}

void Camera::Update()
{
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
	*forward = XMVector3Normalize(focus - *position);
}

void Camera::AttachTo(Entity* entity, float attachedDist)
{
	if (positionState != ATTACHED)
		return;

	delete position; position = &entity->position;
	//delete forward; forward = &entity->for
	this->attachedDist = attachedDist;
}

void Camera::SetUnitVectors()
{
	*forward = XMVector3Normalize(*forward);

	*right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), *forward);
	*right = XMVector3Normalize(*right);

	*up = XMVector3Cross(*forward, *right);
	*up = XMVector3Normalize(*up);

	// roll - rotate on the z axis
	*up = XMVectorSetX(*up, sin( XMConvertToRadians(*roll) ));
	*up = XMVectorSetY(*up, cos( XMConvertToRadians(*roll) ));

	*up = XMVector3Normalize(*up);
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

	XMVECTOR eye = *position - *forward * attachedDist;
	XMVECTOR focus = eye + *forward;

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(
		XMMatrixLookAtLH(eye, focus, *up)));
}