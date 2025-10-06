#pragma once
#include "CoreMinimal.h"

class Area
{
public:
	static const int LT = 0x0001;
	static const int RT = 0x0010;
	static const int LB = 0x0100;
	static const int RB = 0x1000;
};

class AreaInterFace
{
public:
	Vector2D Rcenter;
	int Rwidth;
	int Rheight;

};