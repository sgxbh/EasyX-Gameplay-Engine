#include "InputComponent.h"
#include<Windows.h>

Vector2D InputComponent::mousePos = { 0, 0 };

void InputComponent::SetMapping(std::string mappingname, int value)
{
	mappings.insert({ mappingname,value });
}

void InputComponent::Update()
{
	POINT point;
	GetCursorPos(&point);
	mousePos = Vector2D(float(point.x), float(point.y));
	for (auto& mapping : mappings) {
		KeyBindInfo& info = callbacks[mapping.first];

		if (GetAsyncKeyState(mapping.second) & 0x8000) {
			if (info.type == Pressed && !info.isPressed)info.func();
			if (info.type == Holding)info.func();
			info.isPressed = true;
		}
		else if (info.isPressed) {
			if (info.type == Pressed)info.isPressed = false;
			else if (info.type == Released) { info.func(); info.isPressed = false; }
		}
	}
}

Vector2D InputComponent::GetMousePosition()
{
	return mousePos;
}

bool InputComponent::IsMouseButtonPressed()
{
	return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
}
