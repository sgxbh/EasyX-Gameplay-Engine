#pragma once
#include"CoreMinimal.h"

class Platform :public Object {
private:
	class RigidBody* rigid;
	class BoxCollider* box;
public:
	Platform();
	void SetSize(Vector2D size);
};
