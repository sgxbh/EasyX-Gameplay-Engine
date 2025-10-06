#pragma once
#include"SpriteRenderer.h"
#include"Animator.h"
#include"Collider.h"
#include"RigidBody.h"
#include"Sprite.h"
#include"Timer.h"
#include"Particle.h"

class Player :public Sprite {
	friend class BattleController;
	Animation ani;
	Animation ani_runR;
	Animation ani_jumpR;
	Animation ani_fall;
	Animator* animator;
	CircleCollider* collider;
	BoxCollider* collider_;
	
	Timer blinkTimer;
	Particle* particle;
	AnimationDelegate animationdelegate;
	
	AnimationDelegate jumpenddelegate;
	AnimationDelegate jumptofall;

	void OnOverlap(Collider* overlapComp, Collider* otherComp, Object* otherActor) {
		
	};
public:
	Player() {
		SpriteRenderer* spriteRenderer = GetComponentByClass<SpriteRenderer>();
		spriteRenderer->SetLayer(3);
		spriteRenderer->SetFilter(true,WHITE);//Ìí¼ÓÂË¾µ
		
		animator = ConstructComponent<Animator>();
		ani.Load("ply");
		ani_runR.Load("player_runR");
		ani_jumpR.Load("player_jumpR");
		ani_fall.Load("player_fall");
		ani.SetInterval(0.17f);
		ani_runR.SetInterval(0.1f);
		ani_jumpR.SetInterval(0.05f);
		ani_fall.SetInterval(0.1f);
		

		animator->Insert("idle", ani);
		animator->Insert("player_runR", ani_runR);
		
		animator->Insert("player_jumpR", ani_jumpR);
		animator->Insert("player_fall", ani_fall);
		animator->SetNode("idle");

		animationdelegate.Bind(this, &Player::MyBlink);
		ani.AddNotification(2, animationdelegate);

		
		
		jumpenddelegate.Bind(this, &Player::JumpEnd);
		jumptofall.Bind(this, &Player::JumptoFall);
		ani_jumpR.AddNotification(3, jumpenddelegate);
		ani_jumpR.AddNotification(5, jumptofall);
		
		/*collider = ConstructComponent<CircleCollider>();
		collider->AttachTo(root);
		collider->SetRadius(82);*/

		/*collider->OnComponentBeginOverlap.Add(this, &Player::OnOverlap);
		collider->SetType(CollisionType::player);*/

		/*collider_ = ConstructComponent<BoxCollider>();
		collider_->AttachTo(root);
		collider_->SetSize({ 139,164 });*/


		//blinkTimer.Bind(2, this, &Player::MyBlink,true);

		/*particle = ConstructComponent<Particle>();
		particle->AttachTo(root);
		particle->Load("particle1");
		particle->SetCapacity(10);
		particle->SetSpeed(100.f);
		particle->SetLayer(2);
		particle->SetLifeCircle(2.f);
		particle->SetInterval(0);*/

		
	}

	virtual void Update()override {
		Sprite::Update();
		/*static Vector2D scale = { 1,1 };
		SetLocalScale(scale *= 1.0005);*/
	}
	void MyBlink() {
		Blink(0.2f, BLACK, 80);
	}

	
	void JumpEnd() {
		if(ani_jumpR.Getinterval()!=0)ani_jumpR.SetInterval(0.f); std::cout << "setinterval" << std::endl;
		
	}

	void JumptoFall() {
		ani_jumpR.SetInterval(0.05f);
		animator->SetNode("player_fall");
		std::cout << "setnode" << std::endl;
	}
};

class Shale :public Object {
public:
	BoxCollider* collider;
	Shale() {
		collider = ConstructComponent<BoxCollider>();
		collider->SetSize({ 40,300 });
		collider->SetType(CollisionType::attackbox);
		collider->SetCollisionMode(CollisionMode::Collision);
	}
};
