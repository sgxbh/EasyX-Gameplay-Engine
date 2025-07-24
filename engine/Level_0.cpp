#include "Level_0.h"
#include"GameStatics.h"
#include"SpriteRenderer.h"
#include"Player.h"
#include"Collider.h"
#include"Platform.h"

Level_0::Level_0()
{
	
	bg0 = GameStatics::CreateObject<Sprite>();
	ground = GameStatics::CreateObject<Sprite>();
	bg0->GetComponentByClass<SpriteRenderer>()->LoadSprite("bg0");
	ground->GetComponentByClass<SpriteRenderer>()->LoadSprite("ground");
	ground->GetComponentByClass<SpriteRenderer>()->SetLayer(1);
	ground->SetLocalPosition({ 0,260 });
	plat1 = GameStatics::CreateObject<Platform>();
	plat1->SetLocalPosition({ 0,200 });
	plat1->SetSize({ 2400,45 });
	plat2 = GameStatics::CreateObject<Platform>();
	plat2->SetLocalPosition({ -100,0 });
	plat2->SetSize({ 200,50 });
	
	
	
}

void Level_0::Update()
{
	
	
}
