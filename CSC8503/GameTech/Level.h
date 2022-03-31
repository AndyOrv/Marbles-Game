#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObjec.h"
//#include "Level.h"

namespace NCL {
	namespace CSC8503 {
		class  Level {
		public:
			Level(GameWorld* gameworld, GameTechRenderer* renderer, PhysicsSystem* physics)
			{
				this->gameworld = gameworld;
				this->renderer = renderer;
				this->physics = physics;
				leaveLevel = false;
			};

			void Reset()
			{
				gameworld = new GameWorld();
				renderer = new GameTechRenderer(*gameworld);
				physics = new PhysicsSystem(*gameworld);
			}

			GameTechRenderer* GetRenderer()
			{
				return renderer;
			}

			GameWorld* GetGameWorld()
			{
				return gameworld;
			}


			PhysicsSystem* GetPhysics()
			{
				return physics;
			}


			virtual void UpdateGame(float dt) {
				renderer->Update(dt);
				Debug::FlushRenderables(dt);
				renderer->Render();
			};

			bool leaveLevel;

		private://yes this is double references. This class was not built to work like this, but the menu has forced my hand at the eleventh hour.
			GameWorld* gameworld;
			GameTechRenderer* renderer;
			PhysicsSystem* physics;
		};
	}
}