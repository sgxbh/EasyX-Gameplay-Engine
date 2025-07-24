#pragma once
#include"CoreMinimal.h"
#include<map>
#include<functional>

enum InputType;
enum KeyCode;

struct KeyBindInfo {
	std::function<void()>func;
	InputType type;
	bool isPressed = false;
};


class InputComponent:public Component
{
	static Vector2D mousePos;
	std::map<std::string, int>mappings;
	std::map<std::string, KeyBindInfo>callbacks;
public:
	void SetMapping(std::string mappingname, int value);


	template<typename T>
	void BindAction(std::string actionName, InputType type, T* obj, void(T::* func)()) {
		if (mappings.find(actionName) != mappings.end()) {
			callbacks.insert({ actionName,{std::bind(func,obj),type,false} });
		}
	}

	virtual void Update()override;
	static Vector2D GetMousePosition();
	static bool IsMouseButtonPressed();
};

enum InputType {
	Pressed,
	Released,
	Holding
};

enum KeyCode {
	VK_LButton = 1,
	VK_RButton,
	VK_MButton = 4,
	VK_WheeledUp,
	VK_WheeledDown,
	VK_A = 0x41,
	VK_S = 0x53,
	VK_D = 0x44,
	VK_W = 0x57,
	VK_Q = 0x51,
	VK_E = 0x45,
	SPACE = 0x20,
};