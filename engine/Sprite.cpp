#include "Sprite.h"
#include"Math.h"

void Sprite::Recover()
{
	blinkFlag = false;
}

Sprite::Sprite()
{
	renderer = ConstructComponent<SpriteRenderer>();
	renderer->AttachTo(root);
}

void Sprite::Update()
{
	Object::Update();
	if (isBlinking) {
		if (!blinkFlag) {
			if (transistionLevel > 0)transistionLevel -= transistionSpeed;
			if (transistionLevel <= 0)isBlinking = false;
		}
		renderer->RemoveFilter();
		if (isBlinking)renderer->AddFilter({ blinkFilter.color,int(transistionLevel),1 });
	}
}

void Sprite::Blink(float duration, COLORREF color, int level)
{
	if (!isBlinking) {
		duration = Math::Clamp(duration, 0.1f, 1.f);
		isBlinking = true;
		blinkFlag = true;
		blinkFilter = { color,level,1 };
		transistionLevel = float(level);
		transistionSpeed = 2 * level * DELTA_TIME / duration;
		recoverTimerHandle.Bind(duration / 2, this, &Sprite::Recover);
	}
}
