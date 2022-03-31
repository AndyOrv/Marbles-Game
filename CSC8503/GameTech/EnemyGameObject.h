#pragma once


#include "..\CSC8503Common\GameObject.h"

#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"

#include "../CSC8503Common/NavigationMesh.h"

#include "../CSC8503Common/NavigationGrid.h"

namespace NCL {
	namespace CSC8503 {
		class EnemyGameObject : public GameObject {
		public:
			EnemyGameObject();

			virtual void Update(float dt);

			void EnemyBehaviourTree();



		protected:
			vector <Vector3 > testNodes;
			//NavigationMesh map(const std::string& t = "MazeNavMesh");
			NavigationPath outPath;

		};
	}
}