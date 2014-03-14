#include "Camera.h"

Camera::Camera(Mount mount, float fov, float nearP, float farP)
{
	this->mount = mount;

	fieldOfView = fov;
	nearPlane = nearP;
	farPlane = farP;

	roll = 0.0f;

	position = new XMVECTOR();
	forward = new XMVECTOR();
	right = new XMVECTOR();
	up = new XMVECTOR();

	attachedDist = 0.0f;

	projMatrix = XMFLOAT4X4();
	viewMatrix = XMFLOAT4X4();
}

Camera::~Camera()
{
	if (mount.position != Mount::State::FIXED)
		delete position;

	if (mount.direction != Mount::State::FIXED)
		delete forward;

	delete right;
	delete up;
}

void Camera::Update()
{
	// Mount stuff here
	switch (mount.position)
	{
	case Mount::State::STATIC:
		// do nothing?
		break;
	case Mount::State::FIXED:
		SetViewMatrix();		// only if view changed
		break;
	case Mount::State::FREE:

		break;
	}

	switch (mount.direction)
	{
	case Mount::State::STATIC:
		// do nothing?
		break;
	case Mount::State::FIXED:

		break;
	case Mount::State::FREE:

		break;
	}

	switch (mount.rotation)
	{
	case Mount::State::STATIC:
		// do nothing?
		break;
	case Mount::State::FIXED:

		break;
	case Mount::State::FREE:

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
	if (mount.position != Mount::State::FIXED)
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
	*up = XMVectorSetX(*up, sin( XMConvertToRadians(roll) ));
	*up = XMVectorSetY(*up, cos( XMConvertToRadians(roll) ));

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