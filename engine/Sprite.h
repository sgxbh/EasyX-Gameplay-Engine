#pragma once
#include"CoreMinimal.h"
#include"SpriteRenderer.h"
#include"Timer.h"

class Sprite :public Object {
	SpriteRenderer* renderer;

	bool isBlinking = false;
	bool blinkFlag = false;
	float transistionLevel = 0;
	float transistionSpeed = 0;
	FilterInfo blinkFilter;
	Timer recoverTimerHandle;
	void Recover();
public:
	Sprite();
	virtual void BeginPlay() { Object::BeginPlay(); };
	virtual void Update();
	/*void LoadSprite(std::string name);*/
	void Blink(float duration = 0.2f, COLORREF color = BLACK, int level = 60);
};
