#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObjec.h"
#include "ScrewBallGame.h"
#include "TutorialGame.h"
#include "MazeGame.h"

#include "PushdownState.h"
#include "PushdownMachine.h"
#include "../../Common/Window.h"
#include "Level.h"

namespace NCL {
	namespace CSC8503 {
		class  MainMenu {
		public:

			MainMenu();
			~MainMenu();
			bool UpdateGame(float dt);

		private:
			Level* data;
			PushdownMachine* menu;
		};


	}
}

