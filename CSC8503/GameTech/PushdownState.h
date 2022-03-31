#pragma once
#include "Level.h"

namespace NCL {
	namespace CSC8503 {
		class PushdownState {
		public:
			enum PushdownResult {
				Push, Pop, NoChange

			};
			PushdownState() {}
			PushdownState(NCL::CSC8503::Level* data) {
				this->data = data;
			}

			virtual ~PushdownState() {}


			virtual PushdownResult OnUpdate(float dt, PushdownState** pushFunc) = 0;
			virtual void OnAwake() {}
			virtual void OnSleep() {}
		protected:
			NCL::CSC8503::Level* data;
		};
	}
}