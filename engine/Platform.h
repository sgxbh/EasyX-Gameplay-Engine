#pragma once
#include"CoreMinimal.h"

class Platform :public Object {

public:
	Platform();
	void SetSize(Vector2D size);

	class RigidBody* rigid;
	class BoxCollider* box;
};
