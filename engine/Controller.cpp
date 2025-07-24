#include "Controller.h"
#include"Camera.h"
#include"Math.h"
#include"Collider.h"

Controller::Controller()
{
	camera = ConstructComponent<Camera>();
	camera->AttachTo(root);
	camera->SetMainCamera();
	inputComponent = ConstructComponent<InputComponent>();
}

void Controller::BeginPlay()
{
	Object::BeginPlay();
	SetupInputComponent(inputComponent);
}

Object* Controller::GetObjectUnderCursor()
{
	Vector2D pos = inputComponent->GetMousePosition();
	int x = int(pos.x) / 100; x = Math::Clamp(x, 0, 15);
	int y = int(pos.y) / 100; y = Math::Clamp(y, 0, 8);

	if (!mainWorld.ColliderZones[x][y].empty()) {
		for (auto it = mainWorld.ColliderZones[x][y].rbegin(); it != mainWorld.ColliderZones[x][y].rend(); ++it) {
			if ((*it)->IsMouseOver())return (*it)->pOwner;
		}
	}
	return nullptr;
}
