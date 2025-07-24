#include "RigidBody.h"
#include"Collider.h"
#include"Math.h"

void RigidBody::RestrictVelocity(Vector2D impactNormal, RigidBody* another)
{
	Vector2D tangentVector = { impactNormal.y, -impactNormal.x };

	Vector2D normalVelocity = Vector2D::ProjectVector(velocity, impactNormal);
	Vector2D tangentVelocity = Vector2D::ProjectVector(velocity, tangentVector);

	float friction = 0.2f;
	

	if (!another)
	{
		if (Vector2D::DotProduct(velocity, impactNormal) < 0)
		{
			float multiplier = (tangentVelocity.Size() - normalVelocity.Size() * friction) / tangentVelocity.Size();
			multiplier = Math::Clamp(multiplier, 0.0f, 1.0f);
			velocity = tangentVelocity * multiplier - normalVelocity;
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

	velocity = normalVelocity + tangentVelocity;
	another->velocity = anotherNormalVelocity + anotherTangentVelocity;
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
				RestrictVelocity(-collider->CollisionHit(another).ImpactNormal, another->rigidAttached);
			}
		}
		for (auto& collider : colliders) {
			if (collider->mode != CollisionMode::Collision)continue;
			for (auto& another : collider->collisions) {
				if (another->mode != CollisionMode::Collision || another->rigidAttached)continue;
				RestrictVelocity(-collider->CollisionHit(another).ImpactNormal);
			}
		}
		velocity.x = Math::Clamp(velocity.x, -maxSpeed_x, maxSpeed_x);
		velocity.y = Math::Clamp(velocity.y, -maxSpeed_y, maxSpeed_y);
		pOwner->AddPosition(velocity * DELTA_TIME);

	}
	else if (velocity != Vector2D(0, 0))velocity = Vector2D(0, 0);
	if (bRotatable)pOwner->AddRotation(angularVelocity * DELTA_TIME);
	
}
