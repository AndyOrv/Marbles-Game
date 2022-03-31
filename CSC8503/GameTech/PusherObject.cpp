#include "PusherObject.h"
#include "EnemyGameObject.h"

NCL::CSC8503::PusherObject::PusherObject(float dis)
{
	counter = 0.0f;
	moveDis = dis; //amount of distance pusher will go up
	fire = false;


}

void NCL::CSC8503::PusherObject::Update(float dt)
{
	if (fire) {
		this->MoveUp(dt);
	}
	counter += dt;
	//std::cout << counter << std::endl;

	GetPhysicsObject()->AddForce({ 0, 9.8f, 0 });//gravtiy counter
}

void NCL::CSC8503::PusherObject::MoveUp(float dt)
{
	GetPhysicsObject()->ApplyLinearImpulse({ 0, 100, 0 });
	counter = 0;
	fire = !fire;
}

void NCL::CSC8503::PusherObject::MoveDown(float dt)
{
	GetPhysicsObject()->ApplyLinearImpulse({ 0, -100, 0 });
	counter += dt;
}
