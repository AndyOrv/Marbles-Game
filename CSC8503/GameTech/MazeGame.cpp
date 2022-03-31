#include "MazeGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/AxisConstraint.h"

#include "../CSC8503Common/NavigationMesh.h"

#include <string>

using namespace NCL;
using namespace CSC8503;

MazeGame::MazeGame(GameWorld* gameworld, GameTechRenderer* renderer, PhysicsSystem* physics) : Level(gameworld, renderer, physics) {
	world = gameworld;
	this->renderer = renderer;
	this->physics = physics;

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	score = 0;
	lives = 6;

	leaveLevel = false;

	//Debug::SetRenderer(renderer);
	InitialiseAssets();
}


void MazeGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
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

	InitCamera();
	InitLevelB();
}

MazeGame::~MazeGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void MazeGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

	UpdateKeys();
	renderer->DrawString("lives:" + std::to_string(lives), Vector2(75, 20)); //Draw debug text at 10,20
	renderer->DrawString("score:" + std::to_string(score), Vector2(75, 40)); //Draw debug text at 10,20
	renderer->DrawString("R to reset Ball", Vector2(10, 20)); //Draw debug text at 10,20

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	if (score != player->getScore()) {
		score = player->getScore();
		std::cout << "score : " << score << std::endl;
	}
	if (player->death) {
		player->death = false;
		player->GetTransform().SetPosition(Vector3(-115, 3, 0));//reset player to start
		player->GetPhysicsObject()->ClearVelocity();
		testPush->GetTransform().SetPosition(Vector3(-10, -10, 0));//reset main firing pin
		lives--;
		std::cout << "Lives Left : " << lives << std::endl;
	}

	SelectObject();
	MoveSelectedObject();
	physics->Update(dt);



	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);

		//Debug::DrawAxisLines(lockedObject->GetTransform().GetMatrix(), 2.0f);
	}

	if (lives == 0) {//PLAYER LOST
		std::cout << "PLAYER LOST" << std::endl;
		Debug::Print("YOU lOST!", Vector2(50, 50));
		leaveLevel = true;
	}
	if (player->win) {
		std::cout << "PLAYER WON" << std::endl;
		Debug::Print("YOU WIN!", Vector2(50, 50));
		renderer->DrawString("finale score:" + std::to_string(score), Vector2(50, 60)); //Draw debug text at 10,20
		renderer->DrawString("PRESS ESC TO EXIT", Vector2(50, 70)); //Draw debug text at 10,20
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void MazeGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject = nullptr;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
		InitLevelA(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::V)) {
		testPush->GetTransform().SetPosition(Vector3(-140, 3.1f, 0));
		testPush2->GetTransform().SetPosition(Vector3(220, 3.1f, 100));

		upPush->GetTransform().SetPosition(Vector3(40, -28, 100));
		upPush2->GetTransform().SetPosition(Vector3(-12, -28, 100));

	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
		player->die();
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void MazeGame::LockedObjectMovement() {
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

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}



void MazeGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

void MazeGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	//world->GetMainCamera()->SetPosition(Vector3(500, 500, 500));
	lockedObject = nullptr;
}

void MazeGame::InitWorld() {//call this function to build a specific level i guess
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	//InitSphereGridWorld(5, 5, 3.5f, 3.5f, 1.0f);

	AddSphereToWorld(Vector3(-50, -62, 165), 1.0f, 1.0f);
	testPush = AddButtonToWorld(Vector3(-50, -70, 200), Vector3(-50, -62, 200), 40, pushDirection::UP);

	spinTest = AddCubeToWorld(Vector3(-50, -50, 100), Vector3(5, 5, 5), 0.01f);//NEEDS TO BE OBB BUT NICE TEST EZ CLAP 

	//InitGameExamples();
	InitDefaultFloor();
	//BridgeConstraintTest();//here
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));//here
}

void NCL::CSC8503::MazeGame::InitLevelA()
{
	world->ClearAndErase();
	physics->Clear();

	//pinball launcher w/the ball
	player = AddSphereToWorld(Vector3(-115, 3, 0), 1.0f, 1.0f);
	player->setObjType(GameObject::Player);

	testPush = AddButtonToWorld(Vector3(-10, -10, 0), Vector3(-150, 3.1f, 0), 40, pushDirection::ACROSS);

	testPush2 = AddButtonToWorld(Vector3(-270, -10, 0), Vector3(230, 3.1f, 100), 60, pushDirection::ACROSS);//second launcher

	//Up push
	upPush = AddButtonToWorld(Vector3(40, -30, 100), Vector3(40, -38, 100), 60, pushDirection::UP);
	upPush2 = AddButtonToWorld(Vector3(40, -30, 100), Vector3(-12, -38, 100), 60, pushDirection::UP);

	InitAFloor();

	AddSphereToWorld(Vector3(-100, -9, 80), 1, 1);
	spinTest = AddAngledCubeToWorld(Vector3(-100, -9, 95), Vector3(5, 3, 50), 0, 0, 0, 0.01f);
	spinTest->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	spinTest->GetPhysicsObject()->SetElasticity(10000);
	AxisConstraint* locker = new AxisConstraint(spinTest, Vector3(0, 0, 0));
	world->AddConstraint(locker);

	AddBonusToWorld(Vector3(50, 4, 0));
	AddBonusToWorld(Vector3(70, 4, 0));
}

void NCL::CSC8503::MazeGame::InitLevelB()
{

	player = AddSphereToWorld(Vector3(16.94, 6, -10.93), 1.0f, 1.0f);
	player->setObjType(GameObject::Player);

	//FLOOR
	AddUnityFloorToWorld(Vector3(-2, 0, -3.7f), Vector3(50, 2, 50));
	//MAIN WALLS
	AddUnityFloorToWorld(Vector3(-2.01, 3.5f, -28.16f), Vector3(50, 5, 1));
	AddUnityFloorToWorld(Vector3(-2.01, 3.5f, 20.75f), Vector3(50, 5, 1));
	AddUnityFloorToWorld(Vector3(-26.5, 3.5f, -3.707f), Vector3(1, 5, 47.8f));
	AddUnityFloorToWorld(Vector3(22.49, 3.5f, -3.707f), Vector3(1, 5, 47.8f));
	//INSIDE WALLS
	AddUnityFloorToWorld(Vector3(5.01, 3.5f, 16.22), Vector3(1, 5, 8.01));
	AddUnityFloorToWorld(Vector3(-20.89, 3.5f, 12.06), Vector3(10, 5, 1));
	AddUnityFloorToWorld(Vector3(0.788, 3.5f, 6.5), Vector3(32.916, 5, 1));
	AddUnityFloorToWorld(Vector3(-3.02, 3.5f, 1.987), Vector3(1, 5, 8));
	AddUnityFloorToWorld(Vector3(16.94, 3.5f, -2.15), Vector3(10, 5, 1));
	AddUnityFloorToWorld(Vector3(-10.98, 3.5f, -6.68), Vector3(29.92, 5, 1));
	AddUnityFloorToWorld(Vector3(16.94, 3.5f, -10.93), Vector3(10, 5, 1));
	AddUnityFloorToWorld(Vector3(4.201, 3.5f, -16.18), Vector3(19.58, 5, 1));
	AddUnityFloorToWorld(Vector3(-11.36, 3.5f, -15.047), Vector3(1, 5, 15.64));

	//AddSphereToWorld(Vector3(16.94, 6, -10.93), 1, 0);
	AddSphereToWorld(Vector3(12, 6, 20), 1, 0);

	TestMeshPathfinding();
	DisplayMeshPathfinding();

}

void MazeGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5; //how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30; // constraint distance
	float cubeDistance = 20; // distance between links

	Vector3 startPos = Vector3(500, 500, 500);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void NCL::CSC8503::MazeGame::InitAFloor()
{
	//Inital launch area
	AddFloorToWorld(Vector3(0, 0, 0), Vector3(150, 2, 10));//floor
	AddFloorToWorld(Vector3(0, 0, 12), Vector3(150, 5, 1));//wall
	AddFloorToWorld(Vector3(0, 0, -12), Vector3(150, 5, 1));//wall

	//deathZone
	deathZone = AddDeathZoneToWorld(-100);

	//first corner
	AddFloorToWorld(Vector3(160.1f, 0, 50), Vector3(10, 2, 60));//floor
	AddAngledFloorToWorld(Vector3(170, 3.1f, -5), Vector3(10, 2, 10), 0, 45, 0);//corner
	AddFloorToWorld(Vector3(172, 0, 50), Vector3(1, 5, 40)); //wall

	//second launcher area
	AddFloorToWorld(Vector3(100, 0, 100), Vector3(50, 2, 10));//floor
	AddFloorToWorld(Vector3(100, 0, 88), Vector3(50, 5, 1));//wall
	AddFloorToWorld(Vector3(100, 0, 112), Vector3(50, 5, 1));//wall

	//tube section
	AddFloorToWorld(Vector3(-20, -20, 100), Vector3(70, 2, 10));//floor
	AddFloorToWorld(Vector3(27, 12.1f, 100), Vector3(2, 30, 10));//wallOne
	AddAngledFloorToWorld(Vector3(40, 55, 100), Vector3(10, 2, 10), 0, 0, -45);
	AddAngledFloorToWorld(Vector3(10, 10, 100), Vector3(20, 1, 10), 0, 0, 45);
	AddFloorToWorld(Vector3(2, 45, 100), Vector3(2, 30, 10));//wallTwo
	AddAngledFloorToWorld(Vector3(-12, 55, 100), Vector3(10, 2, 10), 0, 0, -45);
	AddAngledFloorToWorld(Vector3(-42, 10, 100), Vector3(20, 1, 10), 0, 0, 45);
	AddFloorToWorld(Vector3(-25, 12.1f, 100), Vector3(2, 30, 10));//
	AddFloorToWorld(Vector3(-80, 45, 100), Vector3(2, 30, 10));//wallTwo
	AddFloorToWorld(Vector3(-20, 80, 100), Vector3(70, 2, 10));//floor

	//spinner zone
	AddFloorToWorld(Vector3(-100, -15, 95), Vector3(50, 2, 50));//floor

	//win zone
	AddWinZoneToWorld(Vector3(-100, -2, 34.5), Vector3(10, 10, 10));
	AddWinZoneToWorld(Vector3(-160.5, -2, 95), Vector3(10, 10, 10));
	AddWinZoneToWorld(Vector3(-100, -2, 155.5), Vector3(10, 10, 10));
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* MazeGame::AddFloorToWorld(const Vector3& position, const Vector3& size) {
	GameObject* floor = new GameObject("floor");

	Vector3 floorSize = size;
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

GameObject* NCL::CSC8503::MazeGame::AddAngledFloorToWorld(const Vector3& position, const Vector3& size, float pitch, float yaw, float roll)
{
	GameObject* floor = new GameObject("angled floor");

	Vector3 floorSize = size;
	OBBVolume* volume = new OBBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(pitch, yaw, roll))
		;

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* NCL::CSC8503::MazeGame::AddUnityFloorToWorld(const Vector3& position, const Vector3& size)
{
	GameObject* floor = new GameObject("unity floor");



	//Vector3 convertPosition = Vector3(position.x*2,position.y*2,-position.z*2);
	Vector3 convertPosition = Vector3(position.x, position.y, -position.z);

	Vector3 floorSize = size;
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize)
		.SetPosition(convertPosition);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game objects

*/
GameObject* MazeGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* MazeGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject("capsule");

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

GameObject* MazeGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject("cube");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* NCL::CSC8503::MazeGame::AddAngledCubeToWorld(const Vector3& position, const Vector3& dimensions, float pitch, float yaw, float roll, float inverseMass)
{
	GameObject* coobe = new GameObject("angled cube");

	Vector3 floorSize = dimensions;
	OBBVolume* volume = new OBBVolume(floorSize);
	coobe->SetBoundingVolume((CollisionVolume*)volume);
	coobe->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position)
		.SetOrientation(Quaternion::EulerAnglesToQuaternion(pitch, yaw, roll))
		;

	coobe->SetRenderObject(new RenderObject(&coobe->GetTransform(), cubeMesh, basicTex, basicShader));
	coobe->SetPhysicsObject(new PhysicsObject(&coobe->GetTransform(), coobe->GetBoundingVolume()));

	coobe->GetPhysicsObject()->SetInverseMass(inverseMass);
	coobe->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(coobe);

	return coobe;
}



//ADDING OBJECTS TO THE WORLD
void MazeGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	//AddFloorToWorld(Vector3(0, -2, 0),0,0,0);
}

void MazeGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void MazeGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols + 1; ++x) {
		for (int z = 1; z < numRows + 1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void MazeGame::InitDefaultFloor() {
	AddAngledFloorToWorld(Vector3(0, -50, 0), Vector3(100, 2, 100), 10, 0, 0);
	AddFloorToWorld(Vector3(100, -40, 100), Vector3(100, 2, 100));
}

void MazeGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* MazeGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* MazeGame::AddEnemyToWorld(const Vector3& position) {
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

GameObject* MazeGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->ishitable();
	apple->setObjType(GameObject::ObjType::Coin);

	world->AddGameObject(apple);

	return apple;
}

GameObject* MazeGame::AddDeathZoneToWorld(float height)
{
	GameObject* death = new GameObject("death");

	Vector3 floorSize = Vector3(500, 2, 500);
	AABBVolume* volume = new AABBVolume(floorSize);
	death->SetBoundingVolume((CollisionVolume*)volume);
	death->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(Vector3(0, height, 0));

	//death->SetRenderObject(new RenderObject(&death->GetTransform(), cubeMesh, basicTex, basicShader));
	death->SetPhysicsObject(new PhysicsObject(&death->GetTransform(), death->GetBoundingVolume()));

	death->GetPhysicsObject()->SetInverseMass(0);
	death->GetPhysicsObject()->InitCubeInertia();

	death->setObjType(GameObject::DEATHZONE);

	world->AddGameObject(death);

	return death;
}

GameObject* NCL::CSC8503::MazeGame::AddWinZoneToWorld(const Vector3& position, const Vector3& size)
{
	GameObject* win = new GameObject("win");

	Vector3 floorSize = size;
	AABBVolume* volume = new AABBVolume(floorSize);
	win->SetBoundingVolume((CollisionVolume*)volume);
	win->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	win->SetRenderObject(new RenderObject(&win->GetTransform(), cubeMesh, basicTex, basicShader));
	win->SetPhysicsObject(new PhysicsObject(&win->GetTransform(), win->GetBoundingVolume()));

	win->GetPhysicsObject()->SetInverseMass(0);
	win->GetPhysicsObject()->InitCubeInertia();
	win->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	win->setObjType(GameObject::WINZONE);

	world->AddGameObject(win);

	return win;
}

PusherObject* MazeGame::AddButtonToWorld(const Vector3& position, const Vector3& position2, float dis, pushDirection x)//side swiper
{
	//AddSphereToWorld(position, 1.0f, 0.0f);
	Vector3 size = Vector3(10.0f, 1.0f, 10.0f);
	GameObject* leash = AddCubeToWorld(position2, size, 0.0f);

	PusherObject* pushr = new PusherObject(1);

	OBBVolume* volume = new OBBVolume(size);
	pushr->SetBoundingVolume((CollisionVolume*)volume);
	pushr->GetTransform()
		.SetScale(size * 2)
		.SetPosition(position);

	pushr->SetRenderObject(new RenderObject(&pushr->GetTransform(), cubeMesh, basicTex, basicShader));
	pushr->SetPhysicsObject(new PhysicsObject(&pushr->GetTransform(), pushr->GetBoundingVolume()));

	pushr->GetPhysicsObject()->SetInverseMass(1.0f);
	pushr->GetPhysicsObject()->InitCubeInertia();

	pushr->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	world->AddGameObject(pushr);


	PositionConstraint* constraint = new PositionConstraint(leash, pushr, dis);
	world->AddConstraint(constraint);

	AxisConstraint* locker;
	if (x == pushDirection::ACROSS) {
		locker = new AxisConstraint(pushr, Vector3(1, 0, 0));
	}
	else {
		locker = new AxisConstraint(pushr, Vector3(0, 1, 0));
	}

	world->AddConstraint(locker);

	return pushr;
}

StateGameObject* MazeGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0.1f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void NCL::CSC8503::MazeGame::TestMeshPathfinding()
{
	string s = "MazeNavMesh";
	NavigationMesh grid(s);
	//NavigationMesh grid();
	NavigationPath outPath;

	Vector3 startPos(16.94, 3.5, -10.93);
	Vector3 endPos(12, 3.5, 20);

	bool found = grid.FindPath(startPos, endPos, outPath);//broekn part

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void NCL::CSC8503::MazeGame::DisplayMeshPathfinding()
{
	for (int i = 1; i < testNodes.size(); ++i) {
		std::cout << "painting" << std::endl;
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}

}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
bool MazeGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

				//DEBUG INFO
				std::cout << selectionObject->GetName() << std::endl;
				//std::cout << selectionObject->getObjType() << std::endl;



				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press Q to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if (selectionObject) {
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void MazeGame::MoveSelectedObject() {
	//renderer->DrawString("Click Force:" + std::to_string(forceMagnitude), Vector2(10, 20)); //Draw debug text at 10,20

	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;
	if (!selectionObject) { return; }

	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(
					ray.GetDirection() * forceMagnitude,
					closestCollision.collidedAt);
			}
		}
	}
}