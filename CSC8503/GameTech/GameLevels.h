#pragma once
#include "../../Common/Window.h"
#include "TutorialGame.h"
#include "../CSC8503Common/PushdownState.h"
#include"../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/NavigationGrid.h"
#include"../CSC8503Common/BehaviourNode.h"
#include"../CSC8503Common/BehaviourAction.h"
#include"../CSC8503Common/BehaviourNodeWithChildren.h"
#include"../CSC8503Common/BehaviourSelector.h"
#include"../CSC8503Common/BehaviourSequence.h"
#include"PauseScreen.h"

using namespace NCL;
using namespace CSC8503;
class GameLevels :public PushdownState 
{
	PushdownResult OnUpdate(float dt, PushdownState** newState)override
	{
		
		game->UpdateGame(dt);
		if (Window::GetKeyboard() -> KeyDown(KeyboardKeys::P)) 
		{	
			*newState = new PauseScreen();
			return PushdownResult::Push;
			
		}
		if (Window::GetKeyboard() -> KeyDown(KeyboardKeys::F2)) {
			std::cout << "Returning to main menu !\n";
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		
		
	}
protected:

	TutorialGame* game = new TutorialGame();
};

