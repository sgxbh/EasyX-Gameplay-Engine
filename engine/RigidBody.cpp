#include "RigidBody.h"
#include"Collider.h"
#include"Math.h"

void RigidBody::RestrictVelocity(Vector2D impactNormal, RigidBody* another)
{
	Vector2D tangentVector = { impactNormal.y, -impactNormal.x };
	

	Vector2D normalVelocity = Vector2D::ProjectVector(velocity, impactNormal);
	Vector2D tangentVelocity = Vector2D::ProjectVector(velocity, tangentVector);
	if (impactNormal.y == 0) {
		int a = 0;
	}

	float friction = 0.2f;
	

	if (!another)
	{
		if (Vector2D::DotProduct(velocity, impactNormal) < 0)
		{
			float multiplier = (tangentVelocity.Size() - normalVelocity.Size() * friction) / tangentVelocity.Size();
			multiplier = Math::Clamp(multiplier, 0.0f, 1.0f);
			Vector2D newVel  = tangentVelocity * multiplier ;

			pendingVelocity += (newVel - velocity);
		}
		return;
	}

	Vector2D anotherNormalVelocity = Vector2D::ProjectVector(another->velocity, impactNormal);
	Vector2D anotherTangentVelocity = Vector2D::ProjectVector(another->velocity, tangentVector);


	/* 弹性碰撞处理逻辑 */
	if (Vector2D::DotProduct(normalVelocity - anotherNormalVelocity, impactNormal) >= 0)return;//确保有相碰的趋势

	float restitution = 1.f;

	Vector2D normalVelocity_ = normalVelocity;
	normalVelocity = ( normalVelocity* (mass - restitution * another->mass) +  anotherNormalVelocity* another->mass* (1 + restitution)) / (mass + another->mass);
	anotherNormalVelocity = (  anotherNormalVelocity* (another->mass - restitution * mass) +   normalVelocity_*(1 + restitution) * mass) / (mass + another->mass);

	Vector2D newVelThis = normalVelocity + tangentVelocity;
	Vector2D newVelAnother = anotherNormalVelocity + anotherTangentVelocity;

	pendingVelocity += (newVelThis - velocity);
	
}



void RigidBody::Update()
{
	
	if (!pOwner)return;
	
	if (bMovable) {
		if (bGravityEnabled)velocity.y += gravity * 100 * DELTA_TIME;

		for (auto& collider : colliders) {
			if (collider->mode != CollisionMode::Collision)continue;
			for (auto& another : collider->collisions) {
				if (another->mode != CollisionMode::Collision)continue;
				HitResult result = collider->CollisionHit(another);
				Penetrations.push_back({ -result.ImpactNormal ,result.penetration });
				RestrictVelocity(-result.ImpactNormal, another->rigidAttached);
				
			}
		}
		
		

	}
	else if (velocity != Vector2D(0, 0))velocity = Vector2D(0, 0);
	if (bRotatable)pOwner->AddRotation(angularVelocity * DELTA_TIME);
	
}

void RigidBody::BeginPlay()
{
	mainWorld.RigidBodies.insert(this);

}

void RigidBody::ApplyPendingVelocity()
{
	if (bMovable) {
		for (auto& p : Penetrations)
		{
			if (p.penetration <= 5.f) continue;
			pOwner->AddPosition(p.ImpactNormal * p.penetration * 0.5f); 
		}
		Penetrations.clear();
		
		velocity += pendingVelocity;
		pendingVelocity = Vector2D(0, 0);

		velocity.x = Math::Clamp(velocity.x, -maxSpeed_x, maxSpeed_x);
		velocity.y = Math::Clamp(velocity.y, -maxSpeed_y, maxSpeed_y);
		pOwner->AddPosition(velocity * DELTA_TIME);
	}
}
