#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <map>
#include <string>
#include "GameState.h"

typedef std::map<std::string, GameState*> StateMap;

class GameStateManager
{
	friend class DX;
public:
	static bool ChangeState(std::string id, bool immediate = true);
	static bool AddState(std::string, GameState*);
	static GameState* GetState(std::string);
	static void Release();

private:
	static StateMap states;
	static GameState* currentState;
};

#endif