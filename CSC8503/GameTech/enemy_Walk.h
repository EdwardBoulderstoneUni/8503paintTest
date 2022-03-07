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
#include"enemy_ChasePlayer.h"
#include"enemy_lookforbonus.h"

namespace NCL
{
	namespace CSC8503
	{
		class enemy_Walk : public PushdownState
		{
		public:
			enemy_Walk(GameWorld* world,TutorialGame* game)
			{
				this->world = game->GetGameWorld();
				this->game = game;
				object = game->GetAIObject_Enemy();
			
			};

			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				game->UpdateGame2(dt);
				TestPathfinding(startPos, endPos);

				const Vector3 direction = GetWayPoint() - object->GetTransform().GetPosition();
				const Vector3 destination = GetWayPoint();

				if (object->GetTransform().GetPosition().x == destination.x
					&& object->GetTransform().GetPosition().z == destination.z
					|| abs((destination - object->GetTransform().GetPosition()).Length() < 5))
				{
					count++;
				}

				object->GetPhysicsObject()->AddForce(direction.Normalised() * Window::GetTimer()->GetTimeDeltaSeconds() * 30.0f);

				//Use a ray to detemine if it sees player
				Vector3 rayDirection;
				Vector3 rayStartPosition;

				rayDirection = (game->GetPlayer()->GetTransform().GetPosition() - object->GetTransform().GetPosition()).Normalised();
				rayStartPosition = object->GetTransform().GetPosition() + rayDirection * 5.0f;

				Ray eyeRay = Ray(rayStartPosition, rayDirection);

				Debug::DrawLine(eyeRay.GetPosition(), destination, Vector4(1, 0, 0, 1));
				Debug::DrawLine(object->GetTransform().GetPosition(), game->GetPlayer()->GetTransform().GetPosition(), Debug::BLACK, dt);
				RayCollision enemySeeSomething;

				player = game->GetPlayer();

				world->Raycast(eyeRay, enemySeeSomething, true);
				objectSaw = (GameObject*)enemySeeSomething.node;

				//If enemy see Player,then it start chase
				if (objectSaw == player)//FIndPlayer
				{
					chasePlayer = true;

					*newState = new enemy_ChasePlayer(object, objectSaw, game, chasePlayer);
					return PushdownResult::Push;
				}

				//If enemy see bonus,then it start trying to get bonus
				if (objectSaw->GetName() == "Bonus")//SeeBonus
				{
					goForBonus = true;
					if (goForBonus)
					{
						//Get Bonus Position;Set it as Vector3::BonusPosition
						*newState = new enemy_lookforbonus(world, game, object, bonusPosition);
						return PushdownResult::Push;
					}
				}

				

				return PushdownResult::NoChange;
			};


			//Create AI Path
			void TestPathfinding(Vector3 startPos,Vector3 endPos)
			{
				
				NavigationGrid grid("TestGrid1.txt");
				playerGrid = grid;
				NavigationPath outPath;

				bool found = grid.FindPath(startPos, endPos, outPath);

				Vector3 pos;
				while (outPath.PopWaypoint(pos)) {
					pos.y = -6;
					testNodes.push_back(pos);
				}
			}
			
			//Find AI waypoint
			Vector3 GetWayPoint()
			{
				pos = testNodes[count];
				return pos;
			}

		
			void OnAwake() override
			{

			};
		protected:
			NavigationGrid playerGrid;
			NavigationPath out_path;

			int count = 0;
			Vector3 bonusPosition;
			Vector3 pos;
			Vector3 startPos = Vector3(80, 0, 20);
			Vector3 endPos = Vector3(85, 0, 80);


			bool chasePlayer = false;
			bool goForBonus = false;

			vector<Vector3> testNodes;

			GameObject* object;
			GameObject* AIEnemy;

			TutorialGame* game;
			GameWorld* world;
			GameObject* objectSaw;
			GameObject* player;
			
			
		};
	};
}