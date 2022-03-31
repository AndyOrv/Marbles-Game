#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObjec.h"
#include "PusherObject.h"
#include "Level.h"

namespace NCL {
	namespace CSC8503 {
		class MazeGame : public Level {
		public:
			MazeGame(GameWorld* gameworld, GameTechRenderer* renderer, PhysicsSystem* physics);
			~MazeGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void InitLevelA();
			void InitLevelB();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();

			void InitAFloor();


			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();


			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& size);
			GameObject* AddAngledFloorToWorld(const Vector3& position, const Vector3& size, float pitch, float yaw, float roll);
			GameObject* AddUnityFloorToWorld(const Vector3& position, const Vector3& size);

			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddAngledCubeToWorld(const Vector3& position, const Vector3& dimensions, float pitch, float yaw, float roll, float inverseMass = 10.0f);

			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			GameObject* AddDeathZoneToWorld(float height);
			int lives;
			GameObject* AddWinZoneToWorld(const Vector3& position, const Vector3& size);

			enum pushDirection { UP, ACROSS };

			PusherObject* AddButtonToWorld(const Vector3& position, const Vector3& position2, float dis, pushDirection x);
			PusherObject* testPush;
			PusherObject* testPush2;
			PusherObject* upPush;
			PusherObject* upPush2;

			GameObject* spinTest;

			GameObject* player;
			int score;
			GameObject* deathZone;

			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject;



			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh* capsuleMesh = nullptr;
			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLShader* basicShader = nullptr;

			//Coursework Meshes
			OGLMesh* charMeshA = nullptr;
			OGLMesh* charMeshB = nullptr;
			OGLMesh* enemyMesh = nullptr;
			OGLMesh* bonusMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject = nullptr;
			Vector3 lockedOffset = Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			vector <Vector3 > testNodes;
			void TestMeshPathfinding();
			void DisplayMeshPathfinding();

		};
	}
}

