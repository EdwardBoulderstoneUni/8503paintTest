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

namespace NCL
{
	namespace CSC8503
	{
		class enemy_lookforbonus :public PushdownState
		{
		public:
			enemy_lookforbonus(GameWorld* world, TutorialGame* game,GameObject* object,Vector3 endPos)
			{
				this->world = world;
				this->game = game;
				this->object = object;
				this->endPos = endPos;
			};
		
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				NavigationGrid grid("TestGrid1.txt");

				NavigationPath outPath;

				bool found = grid.FindPath(object->GetTransform().GetPosition(), endPos, outPath);

				Vector3 pos;
				while (outPath.PopWaypoint(pos)) {
					wayToBonus.push_back(pos);
				}



				return PushdownResult::NoChange;
			}
		protected:
			GameWorld* world;
			GameObject* object;
			TutorialGame* game;

			Vector3 endPos;
			vector<Vector3> wayToBonus;
		};
	}
}


