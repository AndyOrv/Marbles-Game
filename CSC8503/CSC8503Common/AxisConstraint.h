#include "Constraint.h"
#include "PhysicsObject.h"
#include "GameObject.h"


namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class TutorialGame;

		class AxisConstraint : public Constraint {
		public:
			AxisConstraint(GameObject* a, Vector3 l) {
				object = a;
				lock = l;
			}
			~AxisConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;
			Vector3 lock;

			//float distance;

		};

	}
}