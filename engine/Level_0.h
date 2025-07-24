#pragma once
#include"CoreMinimal.h"
#include"UserInterface.h"


class Level_0 :public Level {
	class Sprite* bg0;
	class Sprite* ground;
	class Platform* plat1;
	class Platform* plat2;
	Characters mycharacters;
public:
	Level_0();
	void Update();
};