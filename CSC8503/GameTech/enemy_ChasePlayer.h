#pragma once
#include "../../Common/Window.h"
#include "TutorialGame.h"
#include "../CSC8503Common/PushdownState.h"
#include"../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/GameObject.h"
#include "../CSC8503Common/NavigationGrid.h"
#include"../CSC8503Common/BehaviourNode.h"
#include"../CSC8503Common/BehaviourAction.h"
#include"../CSC8503Common/BehaviourNodeWithChildren.h"
#include"../CSC8503Common/BehaviourSelector.h"
#include"../CSC8503Common/BehaviourSequence.h"
#include"GameLevel1.h"
#include "enemy_Walk.h"

namespace NCL
{
	namespace CSC8503
	{
		class enemy_ChasePlayer :public PushdownState
		{
		public:
			enemy_ChasePlayer(GameObject* enemy,GameObject* player,TutorialGame* game,bool continueChase)
			{
				this->game = game;
				this->enemy = enemy;
				this->player = player;
				this->continueChase = continueChase;
			}

			

			void TestPathfinding(Vector3 startPos, Vector3 endPos)
			{
				NavigationGrid grid("TestGrid1.txt");
				bool found = grid.FindPath(startPos, endPos, pathToPlayer);

				Vector3 pos;
				while (pathToPlayer.PopWaypoint(pos)) {
					nodes_pathToPlayer.push_back(pos);
				}
			}


			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdateGame(dt);
				//Initial pathFind vector
				Vector3 startPos = enemy->GetTransform().GetPosition();
				Vector3 endPos = player->GetTransform().GetPosition();

				Debug::DrawLine(startPos,endPos,Vector4(1,0,0,1));

				//Form path
				TestPathfinding(startPos,endPos);

				//if enemy see player,then it go straight to him;
				Vector3 direaction =(endPos - startPos).Normalised();//?

				if (continueChase)
				{
					enemy->GetPhysicsObject()->AddForce(direaction * Window::GetTimer()->GetTimeDeltaSeconds() * 100.0f);
					chaseTime -= Window::GetTimer()->GetTimeDeltaSeconds();
				}
				if (chaseTime == 0)
				{

					return PushdownResult::Pop;
				}

				return PushdownResult::NoChange;
			}
			void  OnAwake() override
			{
				
			}
		protected:
			bool continueChase;
			int chaseTime = 5;
			GameObject* enemy = nullptr;
			GameObject* player = nullptr;
			TutorialGame* game;
			NavigationPath pathToPlayer;
			vector<Vector3> nodes_pathToPlayer;
		};

	}
}


