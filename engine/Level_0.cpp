#include "Level_0.h"
#include"GameStatics.h"
#include"SpriteRenderer.h"
#include"Player.h"
#include"Collider.h"
#include"Platform.h"
#include "TestBall.h"
#include "Math.h"
#include "Timer.h"

Level_0::Level_0()
{
	
	bg0 = GameStatics::CreateObject<Sprite>();
	ground = GameStatics::CreateObject<Sprite>();
	bg0->GetComponentByClass<SpriteRenderer>()->LoadSprite("bg0");
	bg0->SetLocalScale({ 3,3 });
	ground->GetComponentByClass<SpriteRenderer>()->LoadSprite("ground");
	ground->GetComponentByClass<SpriteRenderer>()->SetLayer(1);
	ground->SetLocalPosition({ 0,260 });
	plat1 = GameStatics::CreateObject<Platform>();
	plat1->SetLocalPosition({ 0,200 });
	plat1->SetSize({ 2400,45 });
	plat2 = GameStatics::CreateObject<Platform>();
	plat2->SetLocalPosition({ 800,20 });
	plat2->SetSize({ 200,50 });
	
	plat3 = GameStatics::CreateObject<Platform>();
	plat3->SetLocalPosition({ -1200,0 });
	plat3->SetSize({ 50,1200 });
	plat3->rigid->SetMess(5000.f);

	plat4 = GameStatics::CreateObject<Platform>();
	plat4->SetLocalPosition({ 1200,0 });
	plat4->SetSize({ 50,1200 });
	plat4->rigid->SetMess(5000.f);

	plat5 = GameStatics::CreateObject<Platform>();
	plat5->SetLocalPosition({ -450,100 });
	plat5->SetSize({ 700,20 });
	plat5->rigid->SetMess(5000.f);
	
	
	T.Bind(1.f,this, &Level_0::Spawn,true);
}

void Level_0::Update()
{
	
	
}

void Level_0::Spawn()
{
	TestBall* ball=GameStatics::CreateObject<TestBall>();
	ball->SetLocalPosition(Vector2D(Math::RandomPerc() * 2000 - 950, Math::RandomPerc() * 2000 - 950));
	TestBox* box = GameStatics::CreateObject<TestBox>();
	box->SetLocalRotation(Math::RandomPerc() * 360);
	box->SetLocalPosition(Vector2D(Math::RandomPerc() * 2000 - 950, Math::RandomPerc() * 2000 - 950));
}
