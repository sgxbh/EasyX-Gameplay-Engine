#include "TestBall.h"
#include"RigidBody.h"
#include"Collider.h"

TestBall::TestBall()
{
	rigid = ConstructComponent<RigidBody>();
	/*rigid->SetMovable(true);*/
	/*rigid->SetGravityEnable(true);*/
	box = ConstructComponent<CircleCollider>();
	box->AttachTo(root);
	box->SetCollisionMode(CollisionMode::Collision);
	box->SetType(CollisionType::b);
	box->SetRadius(20.f);
	T.Bind(20.f, dynamic_cast<Object*>(this), &TestBall::Destroy);
}

TestBox::TestBox()
{
	rigid = ConstructComponent<RigidBody>();
	/*rigid->SetMovable(true);
	rigid->SetGravityEnable(true);*/
	box = ConstructComponent<BoxCollider>();
	box->AttachTo(root);  
	box->SetCollisionMode(CollisionMode::Collision);
	box->SetType(CollisionType::b);
	box->SetSize({ 40,40 });
	T.Bind(20.f, dynamic_cast<Object*>(this), &TestBall::Destroy);
}