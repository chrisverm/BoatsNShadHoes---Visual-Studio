#ifndef CANNONBALL_H
#define CANNONBALL_H

#include "MoveableEntity.h"
#include "Hittable.h"
#include "ResourceManager.h"
#include "AudioPlayer.h"

class Game;

class CannonBall : public MoveableEntity
{
public:
	CannonBall(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);
	~CannonBall(void);

	void Update(float dt, const XMMATRIX& parentMat);
	void Render(ID3D11DeviceContext* deviceContext);
#if defined(DEBUG) | defined(_DEBUG)
	void DebugRender(ID3D11DeviceContext* deviceContext);
#endif

	void SetVelocity(XMVECTOR);
	bool Active() const;

	void Fire(XMVECTOR position, XMVECTOR acceleration, Hittable* target, float damage);

	static std::vector<AudioPlayer*> splashes;

private:
	bool active;
	float damage;

	Hittable* target;

	float GetYFromXZ(XMVECTOR pos, float time);
	void PlaySplash();

};
#endif