#include "MainMenu.h"
using namespace NCL;
using namespace CSC8503;


class InGameA : public PushdownState {
public:
	InGameA(Level* data) : PushdownState(data) {};

	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		game->UpdateGame(dt);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			std::cout << "Esc";
			return PushdownResult::Pop;
		}
		if (game->leaveLevel) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		game = new MazeGame(data->GetGameWorld(), data->GetRenderer(), data->GetPhysics());
		std::cout << "Game Loaded";
	}

	void OnSleep() override {
		delete game;
		data->GetGameWorld()->ClearAndErase();

	}
protected:
	Level* game;

};

class InGameB : public PushdownState {
public:
	InGameB(Level* gameData) : PushdownState(gameData) {};

	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		game->UpdateGame(dt);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			std::cout << "Esc";
			return PushdownResult::Pop;
		}
		if (game->leaveLevel) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		game = new ScrewBallGame(data->GetGameWorld(), data->GetRenderer(), data->GetPhysics());
		std::cout << "Game Loaded";
	}

	void OnSleep() override {
		delete game;
		data->GetGameWorld()->ClearAndErase();

	}
protected:
	Level* game;

};


class LevelSelect : public PushdownState {
public:
	LevelSelect(Level* gameData) : PushdownState(gameData) {};

	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {



		data->GetRenderer()->DrawString("Level Select", Vector2(10, 10));
		data->GetRenderer()->DrawString("Level AI Maze", Vector2(30, 30), !selected ? Vector4(1, 1, 1, 1) : Vector4(0, 1, 0, 1));
		data->GetRenderer()->DrawString("Level ScrewBall Game", Vector2(30, 40), selected ? Vector4(1, 1, 1, 1) : Vector4(0, 1, 0, 1));
		data->GetRenderer()->DrawString("Press Enter to play", Vector2(10, 60));
		data->GetRenderer()->DrawString("Press Escape to exit", Vector2(10, 70));

		data->UpdateGame(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) {
			selected = !selected;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
			if (selected)
			{
				std::cout << "GameA";
				*newState = new InGameA(data);
			}
			else
			{
				std::cout << "GameB";
				*newState = new InGameB(data);
			}
			return PushdownResult::Push;

		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			std::cout << "Esc";
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;

	};
	void OnAwake() override {


	}
protected:
	bool selected = true;
};

class MainScreen : public PushdownState {
public:
	MainScreen(Level* gameData) : PushdownState(gameData) {};

	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		data->GetRenderer()->DrawString("Main Menu", Vector2(10, 10));
		data->GetRenderer()->DrawString("Press enter for level Select", Vector2(10, 20));
		data->GetRenderer()->DrawString("Press esc to exit", Vector2(10, 30));

		data->UpdateGame(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
			*newState = new LevelSelect(data);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;

		}

		return PushdownResult::NoChange;

	};

	void OnAwake() override {


	}


};



NCL::CSC8503::MainMenu::MainMenu()
{
	GameWorld* world = new GameWorld();
	GameTechRenderer* renderer = new GameTechRenderer(*world);
	PhysicsSystem* physics = new PhysicsSystem(*world);
	Debug::SetRenderer(renderer);
	data = new Level(world, renderer, physics);
	menu = new PushdownMachine((new MainScreen(data)));
}

NCL::CSC8503::MainMenu::~MainMenu()
{
	delete data;
}

bool NCL::CSC8503::MainMenu::UpdateGame(float dt)
{
	//std::cout << "updatingLevel";
	return !menu->Update(dt);
}
