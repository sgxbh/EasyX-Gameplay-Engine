#pragma once
#include"CoreMinimal.h"


class GameStatics {
public:
	static GameInstance* GetGameInstance();
	static Controller* GetController();

	template<typename T>
	static T* CreateObject(Vector2D pos = Vector2D(0, 0));

	template<typename T>
	static std::vector<T*>FindObjectsOfClass();

	template<typename T>
	static T* CreateUI();
};





template<typename T>
inline T* GameStatics::CreateObject(Vector2D pos)
{
	T* pObj = new T;
	if (pObj && static_cast<Object*>(pObj)) {
		mainWorld.GameObjects_to_add.push_back(pObj);
		pObj->SetLocalPosition(pos);
		return pObj;
	}
	delete pObj;
	return nullptr;
}

template<typename T>
inline std::vector<T*> GameStatics::FindObjectsOfClass()
{
	std::vector<T*> result;
	result.reserve(mainWorld.GameObjects.size());
	for (auto& obj : mainWorld.GameObjects) {
		if (T* pObj = static_cast<T*>(obj))result.push_back(pObj);
	}
	return result;
}

template<typename T>
inline T* GameStatics::CreateUI() {
	T* pUI = new T;
	if (pUI && static_cast<UserInterface*>(pUI)) {
		mainWorld.GameUIs.insert(pUI);
		return pUI;
	}
	delete pUI;
	return nullptr;
}