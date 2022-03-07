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
#include"GameLevel1.h"
#include"GameLevel2.h"

using namespace NCL;
using namespace CSC8503;

class GameMenu:public PushdownState
{
public:
	
	GameMenu(TutorialGame* game)
	{
		this->game = game;
	}
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{

		game->UpdateMenu(dt);
		
		

		//Use buttom to Switch level in menu
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
		{
			*newState = new GameLevel1(game);
			game->InitWorld1();
			return PushdownResult::Push;

		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) 
		{
			return PushdownResult::Pop;
		}
		if(Window::GetKeyboard()->KeyPressed(KeyboardKeys::B))
		{
			*newState = new GameLevel2(game);
			game->InitWorld2();
			return PushdownResult::Push;
		}
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		
		//Debug::Print("Welcome to a really awesome game !\n", Vector2(10, 10));
		std::cout << " Press Space To Begin or escape to quit !\n";
		
	}
	
protected:
	TutorialGame* game;

};

