#pragma once
#include "BattleController.h"
#include "GameStatics.h"
#include"Player.h"
#include"Animator.h"
#include"RigidBody.h"
#include"Collider.h"
#include"Particle.h"



BattleController::BattleController()
{
	SetLocalPosition(Vector2D(0, -200));
	//ShakeTimer.Bind(3.f,this, &BattleController::Shake,true);
	ply = GameStatics::CreateObject<Player>();
	ply->SetLocalPosition(Vector2D(0, -165));
	rigidbody = ConstructComponent<RigidBody>();
	rigidbody->SetGravity(98.f);
	rigidbody->SetGravityEnable(true);
	
	collider = ConstructComponent<BoxCollider>();
	collider->AttachTo(root);
	collider->SetSize({ 70,150 });
	collider->SetLocalPosition({ 0,6 });
	
	

	collider_ = ConstructComponent<BoxCollider>();
	collider_->AttachTo(root);
	collider_->SetSize({ 133,165 });
	

	collider->OnComponentBeginOverlap.Add(this, &BattleController::OnOverlap);

	collider->OnComponentEndOverlap.Add(this, &BattleController::EndOverlap);
	collider_->SetType(CollisionType::player);
	collider_->SetCollisionMode(CollisionMode::Collision);
	collider_->OnComponentHit.Add(this, &BattleController::Land);

	particle = ConstructComponent<Particle>();
	particle->AttachTo(root);
	particle->Load("walk_or_land");
	particle->SetCapacity(5);
	particle->SetCenter({ 0,50 }, { 225,315 });
	particle->SetSpeed(100.f);
	particle->SetLayer(2);
	particle->SetLifeCircle(0.5f);
	particle->SetInterval(0);
	particle->SetLocalPosition({ 0,82 });
	particle->SetLocalScale({ 0.2,0.2 });
	particle->SetFadeTime(0.3f);
}

void BattleController::SetupInputComponent(InputComponent* inputComponent)
{
	Controller::SetupInputComponent(inputComponent);

	inputComponent->SetMapping("MoveRight", VK_D);
	inputComponent->SetMapping("MoveRight_start", VK_D);
	inputComponent->SetMapping("MoveRight_end", VK_D);
	inputComponent->BindAction("MoveRight_start", Pressed, this, &BattleController::MoveRightStart);
	inputComponent->BindAction("MoveRight", Holding, this, &BattleController::MoveRight);
	inputComponent->BindAction("MoveRight_end", Released, this, &BattleController::MoveRightEnd);  

	inputComponent->SetMapping("MoveUp", VK_W);
	inputComponent->BindAction("MoveUp", Holding, this, &BattleController::MoveUp);

	inputComponent->SetMapping("MoveDown", VK_S);
	inputComponent->BindAction("MoveDown", Holding, this, &BattleController::MoveDown);

	inputComponent->SetMapping("MoveLeft", VK_A);
	inputComponent->SetMapping("MoveLeft_start", VK_A);
	inputComponent->SetMapping("MoveLeft_end", VK_A);
	inputComponent->BindAction("MoveLeft_start", Pressed, this, &BattleController::MoveLeftStart);
	inputComponent->BindAction("MoveLeft", Holding, this, &BattleController::MoveLeft);
	inputComponent->BindAction("MoveLeft_end", Released, this, &BattleController::MoveLeftEnd);

	inputComponent->SetMapping("ZoomIn", VK_Q);
	inputComponent->BindAction("ZoomIn", Holding, this, &BattleController::ZoomIn);

	inputComponent->SetMapping("ZoomOut", VK_E);
	inputComponent->BindAction("ZoomOut", Holding, this, &BattleController::ZoomOut);

	inputComponent->SetMapping("Jump_start", SPACE);
	inputComponent->BindAction("Jump_start", Pressed, this, &BattleController::JumpStart);
	inputComponent->SetMapping("Jump", SPACE);
	inputComponent->BindAction("Jump", Holding, this, &BattleController::Jump);
	inputComponent->SetMapping("Jump_end", SPACE);
	inputComponent->BindAction("Jump_end", Released, this, &BattleController::JumpEnd);
}

void BattleController::Update()
{
	Controller::Update();
	ply->SetLocalPosition(GetWorldPosition());
}

void BattleController::Shake()
{
	std::cout << "timer run" << std::endl;
	camera->ShakeCamera(50.f,50);
}

void BattleController::MoveRight()
{
	/*AddPosition(Vector2D(0.5, 0));*/
	if (ply->animator->GetNode() != &(ply->ani_runR) && bGround)ply->animator->SetNode("player_runR");
	if (ply->GetLocalScale() != Vector2D(1, 1)&&rigidbody->velocity.x>0)ply->SetLocalScale(Vector2D(1, 1));
	if (rigidbody->velocity.x == 0)rigidbody->SetVelocity(Vector2D(500.f, rigidbody->velocity.y));
	if (!particle->GetIsLoop()&& bGround)particle->SetCapacity(5); particle->SetIsLoop(true); particle->SetCenter({ 0,50 }, { 210,270 });
}

void BattleController::MoveRightStart()
{
	if(bGround)ply->animator->SetNode("player_runR");
	rigidbody->SetVelocity(Vector2D(500.f, rigidbody->velocity.y));
	ply->SetLocalScale(Vector2D(1, 1));
}

void BattleController::MoveRightEnd()
{
	if (bGround)ply->animator->SetNode("idle");
	rigidbody->SetVelocity(Vector2D(0.f, rigidbody->velocity.y));
	particle->SetIsLoop(false);
}

void BattleController::MoveLeft()
{
	if (ply->GetLocalScale() != Vector2D(-1, 1) && rigidbody->velocity.x < 0)ply->SetLocalScale(Vector2D(-1, 1));
	if (ply->animator->GetNode() != &(ply->ani_runR) && bGround)ply->animator->SetNode("player_runR");
	if(rigidbody->velocity.x==0)rigidbody->SetVelocity(Vector2D(-500.f, rigidbody->velocity.y));
	if(!particle->GetIsLoop()&& bGround)particle->SetCapacity(5); particle->SetIsLoop(true); particle->SetCenter({ 0,50 }, { 270,330 });
	
}

void BattleController::MoveLeftStart()
{
	
	if (bGround)ply->animator->SetNode("player_runR");
	rigidbody->SetVelocity(Vector2D(-500.f, rigidbody->velocity.y));
	ply->SetLocalScale(Vector2D(-1, 1));
}

void BattleController::MoveLeftEnd()
{
	if (bGround)ply->animator->SetNode("idle");
	rigidbody->SetVelocity(Vector2D(0.f, rigidbody->velocity.y));
	particle->SetIsLoop(false);
}

void BattleController::MoveUp()
{
	AddPosition(Vector2D(0, -1));
}

void BattleController::MoveDown()
{
	AddPosition(Vector2D(0, 1));
}

void BattleController::ZoomIn()
{
	camera->SetSpringArmLength(ArmLength -= 0.05f);
}

void BattleController::ZoomOut()
{
	camera->SetSpringArmLength(ArmLength += 0.05f);
}

void BattleController::JumpStart()
{
	rigidbody->AddPulse(Vector2D(0, -750.f));
	rigidbody->SetGravityEnable(false);
	ply->animator->SetNode("player_jumpR");
	ply->ani_jumpR.SetInterval(0.05f);
	bGround = false;
	bFalling = false;
	JumpTimer.Continue();
	JumpTimer.Reset();
	particle->SetIsLoop(true);
	particle->SetCapacity(0);
}

void BattleController::Jump()
{
	if (!bFalling && JumpTimer.GetDelay() > 0.5)JumpEnd(); return;
	rigidbody->AddPulse(Vector2D(0, -1.f));
}

void BattleController::JumpEnd()
{
	if (!bFalling) {
		rigidbody->AddPulse(Vector2D(0, rigidbody->velocity.y / 8));
		rigidbody->SetGravityEnable(true);
		ply->ani_jumpR.SetInterval(0.05f);
		bFalling = true;
		std::cout << "jumpend" << std::endl;
	}
	
}

void BattleController::OnOverlap(Collider* overlapComp, Collider* otherComp, Object* otherActor)
{
	rigidbody->AddPulse(Vector2D(0, -rigidbody->velocity.y));
	std::cout << "collide" << std::endl;
	
}

void BattleController::EndOverlap(Collider* overlapComp, Collider* otherComp, Object* otherActor)
{
	/*rigidbody->AddPulse(Vector2D(0, 300.f));*/
	std::cout << "leave" << std::endl;
	
}

void BattleController::Land(Collider* hitComp, Collider* otherComp, Object* otherActor, Vector2D normalImpulse, const HitResult& hitResult)
{
	if (normalImpulse == Vector2D(0, -1)) {
		ply->animator->SetNode("idle");
		bGround = true;
		bFalling = false;
		particle->SetInterval(0);
		particle->SetCapacity(5);
		particle->SetCenter({ 0,50 }, { 225,315 });
		particle->SetIsLoop(false);
	}
	
	
	
}


