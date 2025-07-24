#pragma once
#include"CoreMinimal.h"


class RigidBody :public Component {
	friend class Collider;
	friend class BattleController;
	
	Vector2D velocity = Vector2D(0, 0);
	float maxSpeed_x = 500.f;
	float maxSpeed_y = 850.f;
	bool bMovable = true;
	float gravity = 9.8f;
	bool bGravityEnabled = true;
	float angularVelocity = 0;
	bool bRotatable = true;
	float mass = 1.f;
	std::unordered_set<Collider*>colliders;
	void RestrictVelocity(Vector2D impactNormal, RigidBody* another = nullptr);
public:
	virtual void Update()override;

	void SetGravity(float gravity) { this->gravity = gravity; }
	void SetVelocity(Vector2D veclocity) { this->velocity = veclocity; }
	void SetAngularVelocity(float angularVelocity) { this->angularVelocity = angularVelocity; }
	void SetMovable(bool movable) { this->bMovable = movable; }
	void SetMess(float m) { mass = m; }
	void AddForce(Vector2D force) { velocity += force * DELTA_TIME / mass; }
	void AddPulse(Vector2D pulse) { velocity += pulse / mass; }
	void SetGravityEnable(bool enable) { bGravityEnabled = enable; };
	void SetRotatable(bool enable) { bRotatable = enable; };
};