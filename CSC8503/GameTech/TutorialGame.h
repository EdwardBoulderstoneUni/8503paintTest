#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include"../CSC8503Common/StateMachine.h"
#include"../CSC8503Common/State.h"
#include"StateGameObject.h"
#include"StateGameObject_custom.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);
			virtual void UpdateMenu(float dt);

			void InitWorld();
			void InitialiseAssets();

			bool enterLevel_first = false;
		protected:
			//Level1 map Environment
			void CreateEnvironment_Level1();

			void InitCamera();
			void UpdateKeys();

			void InitFreeCamera();//Freelook Camera
			void InitFixedCamera();//FixedPosition Camera;

			void InitMenu();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();
			void BridgeConstraintTest_custom();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddMenuToWorld(const Vector3& position);

			//Sphere Part
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddObjectSlowerToWorld_sphere(const Vector3& position, float radius,bool triggerValue,float inverseMass = 10.0f);
			GameObject* AddObjectFasterToWorld_sphere(const Vector3& position, float radius,bool triggerValue,float inverseMass = 10.0f);


			//Cube Part
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddUseableCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddBoosterCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddButtomCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 0.0f);
			GameObject* AddRotatingCubeToWorld(const Vector3& position, Vector3 dimensions, float dt,float inverseMass = 10.0f);


			//Capsule Part
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			//Environment Part
			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject = nullptr;

			StateGameObject_custom* AddCustomStateObjectToWorld(const Vector3& position);
			StateGameObject_custom* testObject = nullptr;

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;
		


			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			GameObject* cube_level1;
			GameObject* cube_level2;
			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			OGLTexture* nyanTex = nullptr;
			OGLTexture* dogTex = nullptr;
			OGLTexture* menuTex = nullptr;
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

		};
	}
}

