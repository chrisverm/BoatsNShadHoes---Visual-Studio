#ifndef BOATSNSHADHOES_CANNONBALL_H
#define BOATSNSHADHOES_CANNONBALL_H

#include "MoveableEntity.h"
#include "Hittable.h"
#include "ResourceManager.h"
#include "AudioPlayer.h"

class Game;

class CannonBall : public MoveableEntity
{
public:
	CannonBall(Mesh*, Material*, ID3D11RasterizerState*, ID3D11DepthStencilState*);

	// Update/Render
	void Update(float dt, const XMMATRIX& parentMat);
	void Render(ID3D11DeviceContext* deviceContext);
#if defined(DEBUG) | defined(_DEBUG)
	void DebugRender(ID3D11DeviceContext* deviceContext);
#endif

	// Fire
	void Fire(XMVECTOR position, XMVECTOR acceleration, std::vector<Hittable*> targets, float damage);
	
	// Getter
	bool Active() const;

	static std::vector<AudioPlayer*> splashes;

private:
	bool active;
	float damage;

	std::vector<Hittable*> targets;

	// Helper
	float GetYFromXZ(XMVECTOR pos, float time);

	// Sound
	void PlaySplash();

};
#endif