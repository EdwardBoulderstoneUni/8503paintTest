#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/NavigationMesh.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include"../CSC8503Common/StateMachine.h"
#include"../CSC8503Common/State.h"
#include"StateGameObject.h"
#include"StateGameObject_custom.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"

#include <fstream>
#include <ostream>

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			virtual void UpdateMenu(float dt);
			virtual void UpdateGame2(float dt);

			void InitWorld1();
			void InitWorld2();
			void InitialiseAssets();

			void SetAIObject_Enemy(GameObject* game_object)
			{
				this->AIEnemy = game_object;
			};

			GameObject* GetPlayer()
			{
				return player;
			}

			GameObject* GetAIObject_Enemy()
			{
				return AIEnemy;
			};

			GameWorld* GetGameWorld()
			{
				return world;
			}

			GameObject* GetBonus()
			{
				return bonus;
			}

			void AchieveBonus()
			{
				score += 5000;
			}

			GameObject* AIEnemy = nullptr;
			GameObject* bonus = nullptr;
			bool enterLevel_first = false;
			GameObject* createAIObject(const Vector3& position, float dimensions, float inverseMass = 10.0f);

			void InitCamera1();
			void InitCamera2();

			//1 for level1, 2 for level2
			int levelback = 0;

			void SetLevelbackFactor(int num){levelback = num;}

			void Paint(Vector3 position = Vector3(0, 0, 0), float radius = 1.0f, float hardness = 0.5f, float strength = 0.5f,
				Vector4 color = Vector4(1, 0, 0, 1));

			int GetLevelBackFactor() { return levelback; }

			int score = 0;

			

		protected:
			//Level1 map Environment
			void CreateEnvironment_Level1();
			void CreateEnvironment_Level2();

			void UpdateKeys();

			void InitFreeCamera();//Freelook Camera
			void InitFixedCamera();//FixedPosition Camera;

			void InitMenu();

			void InitGameExamples();


			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void InitDefaultFloorInLevel2();
			void BridgeConstraintTest();
			void BridgeConstraintTest_custom(Vector3 cube,Vector3 rotateCube);
			void OBBConstraint(GameObject* gameobject);

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSmallFloorToWorld(const Vector3& position);
			GameObject* AddKillFloorToWorld(const Vector3& position);
			GameObject* AddMenuToWorld(const Vector3& position);

			//Sphere Part
			GameObject* AddPlayerToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddObjectSlowerToWorld_sphere(const Vector3& position, float radius,bool triggerValue,float inverseMass = 10.0f);
			GameObject* AddObjectFasterToWorld_sphere(const Vector3& position, float radius,bool triggerValue,float inverseMass = 10.0f);


			//Cube Part
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddWinCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddUseableCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddBoosterCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddButtomCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddRotatingCubeToWorld(const Vector3& position, Vector3 dimensions, float dt,float inverseMass = 10.0f);
			GameObject* AddInitialCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddInitialinverseCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);

			//Capsule Part
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			//Environment Part
			GameObject* AddPlayerToWorld(const Vector3& position) const;
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject = nullptr;

			StateGameObject_custom* AddCustomStateObjectToWorld(const Vector3& position);
			StateGameObject_custom* testObject = nullptr;

			GameTechRenderer*	renderer = nullptr;
			PhysicsSystem*		physics = nullptr;
			GameWorld*			world = nullptr;

			bool useGravity;
			bool inSelectionMode;
		


			float		forceMagnitude;

			GameObject* player = nullptr;
			GameObject* selectionObject = nullptr;
			GameObject* cube_level1 = nullptr;
			GameObject* cube_level2 = nullptr;
			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			OGLTexture* nyanTex = nullptr;
			OGLTexture* dogTex = nullptr;
			OGLTexture* menuTex = nullptr;
			OGLTexture* WinTex = nullptr;
			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			float starttime = 0;


			//Paint part
			float _Radius;
			float _Strength;
			float _Hardness;
			Vector3 _PaintPosition;
			Vector4 _PaintColor;


		};

		
	}
}

