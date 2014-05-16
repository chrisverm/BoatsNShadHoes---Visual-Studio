#include "Entity.h"
#include "Camera.h"

#pragma region Construct/Destruct

/*
Constructor

Sets defaults for pos, rot, scale, fwd, right, and up.
*/
Entity::Entity()
	: Forward(forward), Up(up), Right(right)
{
	position = XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f);
	rotation = XMVectorSet(0.0f, 0.0f, 0.0f ,0.0f);
	forward	 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	scale	 = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	up		 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	right	 = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	minAngleClamps = XMVectorSet(-360.0f, -360.0f, -360.0f, -1.0f);
	maxAngleClamps = XMVectorSet( 360.0f,  360.0f,  360.0f,  1.0f);

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

/*
Destructor, loops through children and deletes them, (ignores cameras because they're handled in camera manager).
Deletes bounds if one was created.
*/
Entity::~Entity() 
{
	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end() ; it++)
	{
		if (dynamic_cast<Camera*>(*it) == NULL)
			delete (*it);
	}

	if (bounds != nullptr)
		delete bounds;
}

#pragma endregion

#pragma region Initialize, Update, Render

/*
Initialize this entity with the constant buffer to use and the buffer data to pass through.
*/
void Entity::Initialize(ID3D11Buffer* modelConstBuffer, VSPerModelData* modelConstBufferData)
{
	this->modelConstBuffer = modelConstBuffer;
	this->modelConstBufferData = modelConstBufferData;
}

/*
Updates this entity to the current timestep.
Needs a reference to the parents world matrix.
*/
void Entity::Update(float dt, const XMMATRIX& parentMat)
{
	rotation = XMVectorClamp(rotation, minAngleClamps, maxAngleClamps);
	rotation = XMVectorModAngles(rotation);

	XMMATRIX trans = XMMatrixTranslationFromVector(position);
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);
	XMMATRIX sca = XMMatrixScalingFromVector(scale);
	XMMATRIX worldMat = sca * rot * trans * parentMat;

	if (bounds != nullptr)
	{
		XMMATRIX boundsScale = XMMatrixScaling(bounds->extents.x, bounds->extents.y, bounds->extents.z);
		XMMATRIX boundsMat = boundsScale * rot * trans * parentMat;

		XMStoreFloat4x4(&bounds->worldMat, XMMatrixTranspose(boundsMat));
	}

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(worldMat));

	UpdateForwardFromRotation();

	SetUnitVectors();

	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end(); it++)
	{ (*it)->Update(dt, worldMat); }
}

/*
Render this entity ( and all of its children ).
Entity dosnt actualy render.
*/
void Entity::Render(ID3D11DeviceContext* deviceContext)
{
	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end() ; it++)
	{ (*it)->Render(deviceContext); }
}

#if defined(DEBUG) | defined(_DEBUG)

#pragma endregion

#pragma region DebugRender

bool Entity::drawCoordinates;
Mesh* Entity::coordinateMesh;
Material* Entity::coordinateMaterial;
/*
Renders any debug elements assosiated with this entity ( and all of its children ).
*/
void Entity::DebugRender(ID3D11DeviceContext* deviceContext)
{
	if (Bounds::drawBounds)
	{
		if (bounds != nullptr)
		{
			SetConstantBuffer(deviceContext, bounds->worldMat);
			
			Bounds::mat->SetShaders(deviceContext);
			Bounds::mesh->SetBuffers(deviceContext);

			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

			deviceContext->DrawIndexed(
				Bounds::mesh->Indices(),0,0);
		}
	}

	if (drawCoordinates)
	{
		if (modelConstBuffer != nullptr)
		{
			SetConstantBuffer(deviceContext, worldMatrix);

			coordinateMaterial->SetShaders(deviceContext);
			coordinateMesh->SetBuffers(deviceContext);

			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			deviceContext->DrawIndexed(
				coordinateMesh->Indices(),0,0);
		}
	}

	for (std::vector<Entity*>::iterator it = children.begin(); it != children.end() ; it++)
	{ (*it)->DebugRender(deviceContext); }
}
#endif

#pragma endregion

#pragma region Orientation

/*
banana banana banana.
*/
void Entity::UpdateForwardFromRotation()
{
	XMMATRIX rot = XMMatrixRotationRollPitchYawFromVector(rotation);

	forward = rot.r[2];
}

/*
banana banana banana.
*/
void Entity::UpdateRotationFromForward()
{
	// for the love of all that is holy, someone improve this
	// (I don't even know if it works 100%, I think it does but ...?)
	XMVECTOR absoluteForward = XMVectorSet(0, 0, 1, 0);

	XMVECTOR xPlane = XMVectorSet(0, XMVectorGetY(forward), XMVectorGetZ(forward), 0);
	XMVECTOR yPlane = XMVectorSet(XMVectorGetX(forward), 0, XMVectorGetZ(forward), 0);
	float xAngle = XMVectorGetX(XMVector3AngleBetweenVectors(absoluteForward, yPlane));
	float yAngle = XMVectorGetY(XMVector3AngleBetweenVectors(absoluteForward, xPlane));

	rotation = XMVectorSetX(rotation, xAngle);
	rotation = XMVectorSetY(rotation, yAngle);
}

/*
banana banana banana.
*/
void Entity::SetUnitVectors()
{
	forward = XMVector3Normalize(forward);

	right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
	right = XMVector3Normalize(right);

	up = XMVector3Cross(forward, right);
	up = XMVector3Normalize(up);
	//// roll - rotate on the z axis
	//up = XMVectorSetX(up, sin( XMConvertToRadians(roll) ));
	//up = XMVectorSetY(up, cos( XMConvertToRadians(roll) ));
	//up = XMVector3Normalize(up);
}

#pragma endregion

#pragma region Parenting

/*
Returns the number of children this entity contains.
*/
int Entity::ChildCount()
{ return children.size(); }

/*
Parents the given entity to this entity.
Warning, currently no check for whether or not its already a child here, or parented elsewhere.
Returns the entity for chaining.
*/
Entity* Entity::AddChild(Entity* child)
{
	//TODO: check if already here.
	children.push_back(child);

	child->parent = this;
	return child;
}

/*
Gets the child at the given index.
*/
Entity* Entity::GetChild(int index)
{
	return children[index];
}

/*
Removes the child at the given index.
Returns it for chaining.
*/
Entity* Entity::RemoveChild(int index)
{
	Entity* child = children[index];

	children.erase(children.begin() + index);
	child->parent = nullptr;

	return child;
}

#pragma endregion


/*
Passes this entity's information to the constant buffer.
*/
void Entity::SetConstantBuffer(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 mat)
{
	modelConstBufferData->model = mat;

	deviceContext->UpdateSubresource(
		modelConstBuffer,
		0,
		NULL,
		modelConstBufferData,
		0,
		0);
}