#include "AxisConstraint.h"

void NCL::CSC8503::AxisConstraint::UpdateConstraint(float dt)
{
	PhysicsObject* physOb = object->GetPhysicsObject();

	Vector3 velocity = physOb->GetLinearVelocity();
	physOb->SetLinearVelocity(velocity * lock);
}
