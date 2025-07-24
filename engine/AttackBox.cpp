#include "AttackBox.h"


AttackBox::AttackBox()
{
	box = ConstructComponent<BoxCollider>();
	box->AttachTo(root);
	box->SetCollisionMode(CollisionMode::None);
	box->SetType(CollisionType::attackbox);
	
	timer_cooldown.Bind(0.03f, this, &AttackBox::SetCollisionMode, false);
	timer_distroy.Bind(0.08f, this, &AttackBox::Destroy, false);

	box->OnComponentBeginOverlap.AddDynamic(this, &AttackBox::OnOverlap);
	box->OnComponentEndOverlap.AddDynamic(this, &AttackBox::OnEndOverlap);
}

void AttackBox::OnOverlap(Collider* hitComp, Collider* otherComp, Object* otherActor)
{
}

void AttackBox::OnEndOverlap(Collider* hitComp, Collider* otherComp, Object* otherActor)
{
}
