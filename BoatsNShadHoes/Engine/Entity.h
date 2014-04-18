#ifndef ENTITY_H
#define ENTITY_H

#include <DirectXMath.h>
#include <vector>
#include "BufferStructs.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class Entity
{
public:
	Entity(bool useRotationMat = true);
	~Entity();

	/*
	Update, called every frame, passed a deltaTime value for how long its been since the last update.
	*/
	virtual void Update(float dt, const XMMATRIX& parentMat);

	/*
	Basic position rotation scale needed to construct world matrix.
	*/
	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR forward;
	XMVECTOR scale;
	float roll;

	/*
	Child management methods.

	Get number of children / child at index.
	Add child to entities children (right now no check for if its already there).
	Remove child by index.
	Get reference to the parent of this entity.

	CAREFUL, if an entity is parented to 2 entities, only one will be called its "parent" but both will call its update.
	*/
	int ChildCount();
	Entity* AddChild(Entity*);
	Entity* GetChild(int);
	Entity* RemoveChild(int);
	Entity* Parent();

	/*
	Call these to convert between rotation angles and the forward vector
	*/
	void UpdateForwardFromRotation();
	void UpdateRotationFromForward();

	void SetUnitVectors();

	/*
	Pointers to vectors that cant be changed through here.
	Represent unit vectors pointing in the local z, y, x axis respectively.
	*/
	const XMVECTOR* Forward;
	const XMVECTOR* Up;
	const XMVECTOR* Right;

protected:
	bool useRotation;

	/*
	4x4 float thats passed into shaders for this entities world matrix.
	*/
	XMFLOAT4X4 worldMatrix;

	/*
	The values for f/u/r, changing these wont actualy change the rotation.
	*/
	XMVECTOR up;
	XMVECTOR right;

	/*
	Children and parent of the entity.
	*/
	std::vector<Entity*> children;
	Entity* parent;

};

#endif