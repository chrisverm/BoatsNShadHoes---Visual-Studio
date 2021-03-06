#include "GameStateManager.h"

StateMap GameStateManager::states;
GameState* GameStateManager::currentState = nullptr;

bool GameStateManager::ChangeState(std::string id, bool immediate)
{
	if (states[id] == nullptr)
		return false;

	if (currentState != nullptr)
	{
		currentState->Unload();

		ResourceManager::Release();
		CameraManager::Release();
	}

	if (states[id]->Initialize())
	{
		currentState = states[id];
		return true;
	}

	return false;
}

bool GameStateManager::AddState(std::string id, GameState* state)
{
	if (states[id] != nullptr || state == nullptr)
		return false;

	states[id] = state;

	return true;
}

GameState* GameStateManager::GetState(std::string id)
{
	return states[id];
}

void GameStateManager::Release()
{
	for (StateMap::iterator it = states.begin(); it != states.end(); it = states.begin())
	{
		delete it->second;
		states.erase(it->first);
	}

	ResourceManager::Release();
}