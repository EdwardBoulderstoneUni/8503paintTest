#include "GameTechRenderer.h"
#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include"StateGameObject.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include"PositionConstraint.h"
#include "../../Common/Assets.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);
	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}


void TutorialGame::InitialiseAssets()
{
	

	auto loadFunc = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	dogTex = (OGLTexture*)TextureLoader::LoadAPITexture("doge.png");
	nyanTex = (OGLTexture*)TextureLoader::LoadAPITexture("nyan.png");
	menuTex = (OGLTexture*)TextureLoader::LoadAPITexture("Menu_Box.png");
	WinTex = (OGLTexture*)TextureLoader::LoadAPITexture("nyan - inverse.png");

	InitCamera1();
	InitWorld1();
}

TutorialGame::~TutorialGame()
{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;
	delete dogTex;
	delete menuTex;
	delete nyanTex;

	delete physics;
	delete renderer;
	delete world;
}

#pragma region UpdateMehthods
void TutorialGame::UpdateGame(float dt)
{
	starttime += dt;
	Debug::Print("start time: " + std::to_string(starttime), Vector2(5, 75));
	if (!inSelectionMode)
	{
		world->GetMainCamera()->UpdateCamera(dt);
	}

	if (testStateObject)
	{
		testStateObject->Update(dt);
	}

	UpdateKeys();
	Debug::Print("Constructing Game", Vector2(5, 80));

	if (useGravity)
	{
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else
	{
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	SelectObject();
	MoveSelectedObject();
	physics->Update(dt);

	if (lockedObject != nullptr)
	{
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler();

		Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateGame2(float dt)
{
	starttime += dt;
	Debug::Print("start time: " + std::to_string(starttime), Vector2(5, 75));
	if (!inSelectionMode)
	{
		world->GetMainCamera()->UpdateCamera(dt);
	}

	if (testStateObject)
	{
		testStateObject->Update(dt);
	}

	UpdateKeys();
	if (useGravity)
	{
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else
	{
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	SelectObject();
	MoveSelectedObject();
	physics->Update(dt);


	if (lockedObject != nullptr)
	{
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetNearPlane(0.1f);
		world->GetMainCamera()->SetFarPlane(500.0f);
		world->GetMainCamera()->SetPitch(-15.0f);
		world->GetMainCamera()->SetYaw(280.0f);
		world->GetMainCamera()->SetPosition(Vector3(-180, 40, 100));

		Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateMenu(float dt)
{
	UpdateKeys();

	Debug::Print("Welcome to a really awesome game!\n", Vector2(10, 10));

	Debug::Print("Constructing Game", Vector2(5, 100));

	Debug::Print("Press Space to Level One", Vector2(27.5, 50));
	Debug::Print("Press B to Level TWO", Vector2(27.5, 80));

	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-90.0f);
	world->GetMainCamera()->SetYaw(180.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 320, 0));
	lockedObject = nullptr;

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
	{
		InitWorld1(); //We can reset the simulation_level1 at any time with F1
		selectionObject = nullptr;
		lockedObject = nullptr;
	}


	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G))
	{
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9))
	{
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10))
	{
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7))
	{
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8))
	{
		world->ShuffleObjects(false);
	}

	if (lockedObject)
	{
		LockedObjectMovement();
	}
	else
	{
		DebugObjectMovement();
	}
}
#pragma endregion

#pragma region InitMethods

void TutorialGame::InitCamera1()
{
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(280.0f);
	world->GetMainCamera()->SetPosition(Vector3(-180, 40, 100));
	lockedObject = nullptr;
}

void TutorialGame::InitCamera2()
{
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-50.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(60, 80, 120));
	lockedObject = nullptr;
}

void TutorialGame::InitMenu()
{
	AddMenuToWorld(Vector3(0, 200, 0));
	//Debug::Print("Constructing Game",Vector2(5,80));

	AddButtomCubeToWorld(Vector3(0, 200, 0), Vector3(40, 5, 10));
	AddButtomCubeToWorld(Vector3(0, 200, -30), Vector3(40, 5, 10));
}

void TutorialGame::InitWorld1()
{
	world->ClearAndErase();
	physics->Clear();

	CreateEnvironment_Level1();

	InitMenu();
}

void TutorialGame::InitWorld2()
{
	world->ClearAndErase();
	physics->Clear();

	CreateEnvironment_Level2();
	InitMenu();
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius)
{
	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing)
{
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2)
			{
				AddCubeToWorld(position, cubeDims);
			}
			else
			{
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing,
									 const Vector3& cubeDims)
{
	for (int x = 1; x < numCols + 1; ++x)
	{
		for (int z = 1; z < numRows + 1; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

//Add Floors
void TutorialGame::InitDefaultFloor()
{
	AddFloorToWorld(Vector3(0, -10, 0));
	AddFloorToWorld(Vector3(200, -10, 0));
	AddFloorToWorld(Vector3(0, -10, 200));
	AddFloorToWorld(Vector3(200, -10, 200));

	//TestFloor
	AddFloorToWorld(Vector3(0, 100, 0));


}

void TutorialGame::InitDefaultFloorInLevel2()
{
	AddSmallFloorToWorld(Vector3(50, -10, 50));
}

GameObject* TutorialGame::AddSmallFloorToWorld(const Vector3& position)
{
	GameObject* floor = new GameObject("Default Floor");

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		 .SetScale(floorSize)
		 .SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();


	world->AddGameObject(floor);

	return floor;
}

void TutorialGame::InitGameExamples()
{
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}
#pragma endregion

#pragma region ConstraintPart

void TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30; // constraint distance
	float cubeDistance = 20; // distance between links

	Vector3 startPos = Vector3(0, 150, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i)
	{
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);

		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::BridgeConstraintTest_custom(Vector3 cube, Vector3 rotateCube)
{
	Vector3 FixedCubeSize = Vector3(4, 4, 4);
	Vector3 RotateCubeSize = Vector3(5, 5, 5);
	float invMass_Fixed = 0;
	float invMass_Rotate = 1;
	float looplength = 30.0f;
	//Only one link
	Vector3 startPos = cube;
	Vector3 RotatePos = rotateCube;

	GameObject* fixedCube = new GameObject("Pile");
	GameObject* RotateCube = new GameObject("CUBE");

	fixedCube = AddCubeToWorld(startPos, FixedCubeSize, invMass_Fixed);
	RotateCube = AddRotatingCubeToWorld(RotatePos,
										RotateCubeSize, Window::GetTimer()->GetTimeDeltaSeconds(), invMass_Rotate);

	RotateCube->GetPhysicsObject()->SetInverseMass(invMass_Rotate);
	RotateCube->GetPhysicsObject()->InitCubeInertia();

	RotateCube->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 500));

	PositionConstraint* constraint = new PositionConstraint(fixedCube, RotateCube, looplength);
	world->AddConstraint(constraint);

	Debug::DrawLine(fixedCube->GetTransform().GetPosition(), RotateCube->GetTransform().GetPosition(),
					Vector4(1, 0, 0, 1));
}

void TutorialGame::OBBConstraint(GameObject* game_object)
{
	float Distance = 5.0;
	Vector3 gameobject_position = game_object->GetTransform().GetPosition();
	GameObject* pin = AddSphereToWorld(
		Vector3(gameobject_position.x, gameobject_position.y + 4.0f, gameobject_position.z)
		, 2.0f, 0.0f);
	PositionConstraint* constraint = new PositionConstraint(game_object, pin, Distance);
	world->AddConstraint(constraint);
}
#pragma endregion

#pragma region BasicGemotary

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position)
{
	GameObject* floor = new GameObject("Default Floor");

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		 .SetScale(floorSize * 2)
		 .SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();


	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddKillFloorToWorld(const Vector3& position)
{
	GameObject* floor = new GameObject("KillFloor");
	floor->SetTrigger(true);
	floor->SetTag("KillFloor");
	Vector3 floorSize = Vector3(400, 2, 400);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		 .SetScale(floorSize * 2)
		 .SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, dogTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();


	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddMenuToWorld(const Vector3& position)
{
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(100, 2, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		 .SetScale(floorSize * 2)
		 .SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, menuTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();


	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject("Player");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, nyanTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject("Sphere_test");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, nyanTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass)
{
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		   .SetScale(Vector3(radius * 2, halfHeight, radius * 2))
		   .SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, dogTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddWinCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("WinBroad");
	cube->SetTrigger(true);
	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, WinTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddBoosterCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("Booster");

	cube->SetTrigger(true);
	cube->SetTag("Booster");
	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddInitialCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("StartCube");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddInitialinverseCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("StartCube2");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

//Need to Use OBBvolume to replace
GameObject* TutorialGame::AddUseableCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("Cube_Useable");
	cube->SetTag("Useable");

	OBBVolume* volume = new OBBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, dogTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddButtomCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass)
{
	GameObject* cube = new GameObject("Buttom");

	AABBVolume* volume = new AABBVolume(dimensions);

	//cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddRotatingCubeToWorld(const Vector3& position, Vector3 dimensions, float dt,
												 float inverseMass)
{
	GameObject* cube = new GameObject("CUBE");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	Vector3 dimensionfactor = Vector3(1, 1, 1);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2 * dimensionfactor);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, dogTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 5 * dt, 0));
	world->AddGameObject(cube);

	return cube;
}


#pragma endregion

#pragma region ObjectVelcioty_Sphere
GameObject* TutorialGame::AddObjectSlowerToWorld_sphere(const Vector3& position, float radius, bool triggerValue,
														float inverseMass)
{
	GameObject* sphere = new GameObject("SlowenSphere");
	sphere->SetTrigger(triggerValue);
	sphere->SetTag("Slower");

	std::cout << sphere->GetTrigger() << std::endl;
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);

	//sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->SetBoundingVolume((CollisionVolume*)volume);


	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	sphere->GetPhysicsObject()->SetTriggerValue(triggerValue);
	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddObjectFasterToWorld_sphere(const Vector3& position, float radius, bool triggerValue,
														float inverseMass)
{
	GameObject* sphere = new GameObject("FastenSphere");
	sphere->SetTrigger(triggerValue);
	sphere->SetTag("Fasters");


	std::cout << sphere->GetTrigger() << std::endl;
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);

	//sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->SetBoundingVolume((CollisionVolume*)volume);


	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}

#pragma endregion

#pragma region EnvironmentDesign
GameObject* TutorialGame::createAIObject(const Vector3& position, float radius, float inverseMass)
{
	GameObject* sphere = new GameObject("Enemy");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, nyanTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}

void TutorialGame::CreateEnvironment_Level1()
{
	testObject = AddCustomStateObjectToWorld(Vector3(5, 5, 5));
	//BridgeConstraintTest();
	//bonus= AddBonusToWorld(Vector3(-75,0,50)); Bonus
	//testAIEnemy = AddSphereToWorld(Vector3(80, 0, 10), 4.0f);
	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	//AddCubeToWorld(Vector3(15, 10, 25), Vector3(3, 5, 2));
	//AddRotatingCubeToWorld(Vector3(5, 8, 5), Vector3(5,5,5),30.0f);
	//InitGameExamples();

	InitDefaultFloor();
	AddKillFloorToWorld(Vector3(0, -20, 0));


	player = AddPlayerToWorld(Vector3(-110, 20, 0), 4.0f); //Player

	GameObject* rotateCube_1 = AddUseableCubeToWorld(Vector3(280, 0, 180), Vector3(16, 4, 8));
	GameObject* rotateCube_2 = AddUseableCubeToWorld(Vector3(-180, 20, 0), Vector3(16, 4, 8));

	OBBConstraint(rotateCube_1);
	OBBConstraint(rotateCube_2);
	//AddObjectSlowerToWorld_sphere(Vector3(25, 10, 25), 8.0f, true);

	//Bonus
	AddBonusToWorld(Vector3(25, 0, 120));
	AddBonusToWorld(Vector3(200, 0, 30));
	AddBonusToWorld(Vector3(80, 0, 50));
	AddBonusToWorld(Vector3(280, 0, 220));


	AddInitialCubeToWorld(Vector3(-110, 0, 0), Vector3(6, 8, 20));
	AddBoosterCubeToWorld(Vector3(-50, 0, 30), Vector3(6, 8, 20));
	AddBoosterCubeToWorld(Vector3(50, 0, 150), Vector3(6, 8, 20));
	AddBoosterCubeToWorld(Vector3(295, 0, 50), Vector3(6, 8, 20));
	AddBoosterCubeToWorld(Vector3(-70, 0, 200), Vector3(6, 8, 20));

	//The Gate
	AddWinCubeToWorld(Vector3(250, 20, 250), Vector3(25, 25, 12.5), 0.0f);

	//Add Wall
	AddCubeToWorld(Vector3(100, 42, -100), Vector3(200, 50, 30), 0.0f);
	AddCubeToWorld(Vector3(100, 42, 300), Vector3(200, 50, 30), 0.0f);
	AddCubeToWorld(Vector3(350, 42, 100), Vector3(50, 50, 200), 0.0f);
	BridgeConstraintTest_custom(Vector3(80, 0, 40), Vector3(70, -3, 40));
	BridgeConstraintTest_custom(Vector3(130, 0, 110), Vector3(145, -3, 40));
	BridgeConstraintTest_custom(Vector3(-70, 0, 130), Vector3(-85, 0, 60));

	AddPlayerToWorld(Vector3(5, 0, 0));
	AddPlayerToWorld(Vector3(0, 0, 0));

	AddObjectFasterToWorld_sphere(Vector3(25, 0, 25), 8.0f, true, 0.0f);
	AddObjectSlowerToWorld_sphere(Vector3(200, 0, 180), 5.0f, true, 0.0);

	//
}

void TutorialGame::CreateEnvironment_Level2()
{
	InitDefaultFloorInLevel2();
	player = AddPlayerToWorld(Vector3(13, 0, 95), 2.0f); //(Player)
	AIEnemy = createAIObject(Vector3(80, -5, 20), 2.0f); //Enemy

	AddInitialinverseCubeToWorld(Vector3(13, -5, 95), Vector3(4, 2, 3), 0.0f);

	AddWinCubeToWorld(Vector3(83.5, -2, 5), Vector3(4.85, 5, 5), 0.0f);
	//Create Maze
	std::ifstream infile(Assets::DATADIR + "TestGrid1.txt");

	int nodeSize;
	int gridWidth;
	int gridHeight;

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	GridNode* allNodes;
	allNodes = new GridNode[gridWidth * gridHeight];

	for (int y = 0; y < gridHeight; ++y)
	{
		for (int x = 0; x < gridWidth; ++x)
		{
			GridNode& n = allNodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			n.type = type;
			n.position = Vector3((float)(x * nodeSize + 3.0f), -4, (float)(y * nodeSize + 3.0f));
			if (type == 'x')
			{
				AddCubeToWorld(n.position, Vector3(4.75, 4, 4), 0);
			}
		}
	}
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) const
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(5.0f, 2.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
			 .SetScale(Vector3(meshSize, meshSize, meshSize))
			 .SetPosition(position);

	if (rand() % 2)
	{
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else
	{
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->GetRenderObject()->SetColour(Vector4(0, 0, 0, 1));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
			 .SetScale(Vector3(meshSize, meshSize, meshSize))
			 .SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position)
{
	GameObject* sphere = new GameObject("Bonus");
	sphere->SetTag("1");
	Vector3 sphereSize = Vector3(2, 2, 2);
	SphereVolume* volume = new SphereVolume(2);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		  .SetScale(sphereSize)
		  .SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetRenderObject()->SetColour(Vector4(95, 98, 95, 1));

	sphere->GetPhysicsObject()->SetInverseMass(1.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();

	//sphere->GetPhysicsObject()->SetAngularVelocity(Vector3(0,0,10));

	world->AddGameObject(sphere);

	return sphere;
}
#pragma endregion

#pragma region StateObject

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		 .SetScale(Vector3(0.25, 0.25, 0.25))
		 .SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

StateGameObject_custom* TutorialGame::AddCustomStateObjectToWorld(const Vector3& position)
{
	StateGameObject_custom* apple = new StateGameObject_custom();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		 .SetScale(Vector3(0.5, 0.5, 0.5))
		 .SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}
#pragma endregion

#pragma region SeletedObjectMovement

bool TutorialGame::SelectObject()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode)
		{
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else
		{
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode)
	{
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1))
		{
			string position = "position (" + std::to_string(selectionObject->GetTransform().GetPosition().x)
				+ "," + std::to_string(selectionObject->GetTransform().GetPosition().y)
				+ "," + std::to_string(selectionObject->GetTransform().GetPosition().z) + ")";
			Debug::Print(position, Vector2(5, 25));
			string speed = "speed (" + std::to_string(selectionObject->GetPhysicsObject()->GetLinearVelocity().x)
				+ "," + std::to_string(selectionObject->GetPhysicsObject()->GetLinearVelocity().y)
				+ "," + std::to_string(selectionObject->GetPhysicsObject()->GetLinearVelocity().z) + ")";
			Debug::Print(speed, Vector2(5, 30));
		}

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT))
		{
			if (selectionObject)
			{
				//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));

				selectionObject = nullptr;
				lockedObject = nullptr;
			}
			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true))
			{
				_PaintPosition = closestCollision.collidedAt;
				_PaintColor = Vector4(1, 0, 0, 1);	//Default test color red
				std::cout << closestCollision.collidedAt << std::endl;

				//Got _PaintPosition,_Radius,_Hardness,_Strength,_PaintColor already
				//Need to find how to paint on the point how to realize if 
				Paint(_PaintPosition, _Radius, _Hardness, _Strength, _PaintColor);

				selectionObject = (GameObject*)closestCollision.node;
				if (selectionObject->GetTag() == "Useable")
				{
					selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 1, 1));
					selectionObject->GetPhysicsObject()->ApplyAngularImpulse(Vector3(0, 10, 0));
					
					/*selectionObject->GetPhysicsObject()->AddForceAtPosition(
						Vector3(0,10,0),
						Vector3(selectionObject->GetTransform().GetPosition().x,
							selectionObject->GetTransform().GetPosition().y,
							selectionObject->GetTransform().GetPosition().z+10));*/
				}
				else if (selectionObject->GetName() == "StartCube")
				{
					selectionObject->SetTag("Start");
				}
				else if (selectionObject->GetName() == "StartCube2")
				{
					selectionObject->SetTag("Start2");
				}
				else
				{
					selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				}

				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject)
	{
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if (selectionObject)
	{
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L))
	{
		if (selectionObject)
		{
			if (lockedObject == selectionObject)
			{
				lockedObject = nullptr;
			}
			else
			{
				lockedObject = selectionObject;
			}
		}
	}

	return false;
}

void TutorialGame::MoveSelectedObject()
{
	renderer->DrawString(" Click Force :" + std::to_string(forceMagnitude), Vector2(10, 20));
	// Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject)
	{
		return;; // we haven ’t selected anything !
	}
	// Push the selected object !
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT))
	{
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true))
		{
			if (closestCollision.node == selectionObject)
			{
				selectionObject->GetPhysicsObject()->AddForceAtPosition(
					ray.GetDirection() * forceMagnitude,
					closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::LockedObjectMovement()
{
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
	{
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
	{
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
	{
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
	{
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT))
	{
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement()
{
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject)
	{
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8))
		{
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5))
		{
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}
#pragma endregion

#pragma region PaintFunction
void TutorialGame::Paint(Vector3 position,float radius, float hardness, float strength,Vector4 color)
{
	renderer->RenderPaint();
}

#pragma endregion
