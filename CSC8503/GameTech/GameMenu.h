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
#include"GameLevels.h"

using namespace NCL;
using namespace CSC8503;

class GameMenu:public PushdownState
{
public:

	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		game->UpdateMenu(dt);
		Debug::Print("Constructing Game", Vector2(5, 100));

		//Use buttom to Switch level in menu
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
		{
			*newState = new GameLevels();
			return PushdownResult::Push;

		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		
		//Debug::Print("Welcome to a really awesome game !\n", Vector2(10, 10));
		std::cout << " Press Space To Begin or escape to quit !\n";
		
	}

protected:
	TutorialGame* game = new TutorialGame();
};

