#include "Platform.h"
#include"RigidBody.h"
#include"Collider.h"

Platform::Platform()
{
	rigid = ConstructComponent<RigidBody>();
	rigid->SetMovable(false);
	rigid->SetGravityEnable(false);
	box = ConstructComponent<BoxCollider>();
	box->AttachTo(root);
	box->SetCollisionMode(CollisionMode::Collision);
	box->SetType(CollisionType::ground);
}

void Platform::SetSize(Vector2D size)
{
	box->SetSize(size);
}
