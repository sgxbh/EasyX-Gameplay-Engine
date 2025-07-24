#pragma once
#include"Controller.h"
#include"Timer.h"
#include"Camera.h"
#include"Collider.h"


class BattleController :public Controller {
	Timer OverallTimeHandle;
	Timer ShakeTimer;
	Timer JumpTimer;

	class Player* ply;
	class RigidBody* rigidbody;
	float ArmLength = 20.f;

	class BoxCollider* collider;
	class BoxCollider* collider_;
	class Particle* particle;

	bool bGround = true;
	bool bFalling = false;
	
public:
	BattleController();
	
	virtual void SetupInputComponent(InputComponent* inputComponent)override;

	void Update();

	void Shake();

	void MoveRight();
	void MoveRightStart();
	void MoveRightEnd();

	void MoveLeft();
	void MoveLeftStart();
	void MoveLeftEnd();

	void MoveUp();
	void MoveDown();

	void ZoomIn();

	void ZoomOut();

	void JumpStart();
	void Jump();
	void JumpEnd();

	void OnOverlap(Collider* overlapComp, Collider* otherComp, Object* otherActor);
	void EndOverlap(Collider* overlapComp, Collider* otherComp, Object* otherActor);
	void Land(Collider* hitComp, Collider* otherComp, Object* otherActor, Vector2D normalImpulse, const HitResult& hitResult);
};