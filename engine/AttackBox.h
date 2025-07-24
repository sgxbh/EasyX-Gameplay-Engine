#pragma once
#include"CoreMinimal.h"
#include"Timer.h"
#include"Collider.h"

class AttackBox :public Object {
	float damage = 3;
	float duration = 0.1f;
	class BoxCollider* box;
	Timer timer_cooldown;
	Timer timer_distroy;
public:
	AttackBox();
	void OnOverlap(class Collider* hitComp, Collider* otherComp, Object* otherActor);

	void OnEndOverlap(class Collider* hitComp, Collider* otherComp, Object* otherActor);

	void Destroy() { Object::Destroy(); };
	void SetCollisionMode() { box->SetCollisionMode(CollisionMode::Trigger); };
};