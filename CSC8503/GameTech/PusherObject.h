#pragma once
#include "..\CSC8503Common\GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class PusherObject : public GameObject {
		public:
			PusherObject(float dis);

			virtual void Update(float dt);

			void setFire() { fire = !fire; }//fire on/off

		protected:
			void MoveUp(float dt);
			void MoveDown(float dt);

			bool fire;

			float counter;
			float moveDis;
		};
	}
}
