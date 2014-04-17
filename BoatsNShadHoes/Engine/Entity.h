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
	Entity();
	~Entity();

	/*
	Update, called every frame, passed a deltaTime value for how long its been since the last update.
	*/
	virtual void Update(float dt, const XMMATRIX& parentMat);

	/*
	Basic position rotation scale needed to construct world matrix.
	*/
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;

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
	Kinda messy, could be re-worked.
	Call these to update the forward,up,right vectors with any changes to rotation.
	*/
	void UpdateOrientation();
	void UpdateOrientation(const DirectX::XMMATRIX&, bool = true);

	/*
	Pointers to vectors that cant be changed through here.
	Represent unit vectors pointing in the local z, y, x axis respectively.
	*/
	const DirectX::XMVECTOR* Forward;
	const DirectX::XMVECTOR* Up;
	const DirectX::XMVECTOR* Right;

protected:
	/*
	4x4 float thats passed into shaders for this entities world matrix.
	*/
	DirectX::XMFLOAT4X4 worldMatrix;

	/*
	The values for f/u/r, changing these wont actualy change the rotation.
	*/
	DirectX::XMVECTOR forward;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR right;

	/*
	Children and parent of the entity.
	*/
	std::vector<Entity*> children;
	Entity* parent;

};

#endif