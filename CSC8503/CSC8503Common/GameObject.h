#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void setGravity() {
				gravity = !gravity;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int		GetWorldID() const {
				return worldID;
			}

			enum ObjType {Player, Coin, Enemy, WINZONE, DEATHZONE};
			ObjType* getObjType() {
				return &type;
			}
			void setObjType(ObjType x) {
				type = x;
			}
			bool isCoin() {
				if (type == Coin) {
					return true;
				}
				return false;
			}

			bool nohit;
			void ishitable() {
				nohit = !nohit;
			}

			int getScore() {
				return collects;
			}
			void addScore() {
				collects++;
			}

			bool death;
			void die() {
				death = true;
			}

			bool win;
			void won() {
				win = true;
			}

			bool done;

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;

			bool	isActive;
			bool gravity;
			int		worldID;
			string	name;
			ObjType type;

			int collects;

			Vector3 broadphaseAABB;
		};
	}
}

