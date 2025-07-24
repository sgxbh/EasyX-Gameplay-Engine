#pragma once
#include "CoreMinimal.h"
#include"InputComponent.h"
class Controller:public Object
{
	InputComponent* inputComponent;
protected:
	class Camera* camera;

	virtual void SetupInputComponent(InputComponent* inputComponent){}
public:
	Controller();

	virtual void BeginPlay()override;

	void PickInfo() { inputComponent->Update(); }

	Vector2D GetCursorPosition()const { return inputComponent->GetMousePosition(); }

	bool IsMouseClicked()const { return InputComponent::IsMouseButtonPressed(); }

	Object* GetObjectUnderCursor();
};

