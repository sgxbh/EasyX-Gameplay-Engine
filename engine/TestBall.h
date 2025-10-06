#pragma once
#include "CoreMinimal.h"
#include "Timer.h"

class TestBall : public Object
{
public:
	TestBall();
	class RigidBody* rigid;
	class CircleCollider* box;
	Timer T;
	
};


class TestBox : public Object
{
public:
	TestBox();
	class RigidBody* rigid;
	class BoxCollider* box;
	Timer T;

};

