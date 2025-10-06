#pragma once
#include"CoreMinimal.h"
#include"UserInterface.h"
#include"Timer.h"


class Level_0 :public Level {
	class Sprite* bg0;
	class Sprite* ground;
	class Platform* plat1;
	class Platform* plat2;
	class Platform* plat3;
	class Platform* plat4;
	class Platform* plat5;
	class Timer T;
	Characters mycharacters;
public:
	Level_0();
	void Update();
	void Spawn();
};