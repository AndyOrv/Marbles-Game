#include "EnemyGameObject.h"

NCL::CSC8503::EnemyGameObject::EnemyGameObject()
{
}

void NCL::CSC8503::EnemyGameObject::Update(float dt)
{
}

void NCL::CSC8503::EnemyGameObject::EnemyBehaviourTree()
{
	BehaviourAction* findPlayer = new BehaviourAction("Find Player",
		[&](float dt, BehaviourState state)->BehaviourState {

			//draw a path towards player location and follow it



			return state;
		});
	BehaviourAction* goPlayer = new BehaviourAction("Move towards Player",
		[&](float dt, BehaviourState state)->BehaviourState {

			//draw a path towards player location and follow it



			return state;
		});
	BehaviourAction* respawn = new BehaviourAction("Respawn",
		[&](float dt, BehaviourState state)->BehaviourState {


			//something has gone wrong, reset postion


			return state;
		});
	BehaviourAction* scanBonuses = new BehaviourAction("Scan Bonuses",
		[&](float dt, BehaviourState state)->BehaviourState {


			//scan for bounse in area
			//draw path

			return state;
		});
	BehaviourAction* collectBonuses = new BehaviourAction("Collect Bonuses",
		[&](float dt, BehaviourState state)->BehaviourState {


			//move towards bonuse on path


			return state;
		});
	BehaviourAction* scanavoid = new BehaviourAction("Scan Obstacle",
		[&](float dt, BehaviourState state)->BehaviourState {


			//scan for obstacles


			return state;
		});

	BehaviourAction* avoid = new BehaviourAction("Avoid Obstacle",
		[&](float dt, BehaviourState state)->BehaviourState {


			//Move to avoid


			return state;
		});



}