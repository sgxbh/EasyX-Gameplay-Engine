#pragma once
#include"Widgets.h"
#include"CoreMinimal.h"

class UserInterface :public Base {
	friend class Panel;
	
	std::unordered_set<Widget*>widgets;
protected:
	Widget* const rootCanvas = new Widget;
public:
	UserInterface() { rootCanvas->SetSize({ WIN_WIDTH,WIN_HEIGHT }); }
	~UserInterface() { for (auto& widget : widgets)delete widget; }

	virtual void Update();

	virtual void BeginPlay(){}

	void ShowInfoBox() { for (auto& widget : widgets) { widget->ShowInfoBox(); } }

	template<class T>
	T* AddWidget()
	{
		T* gameWid = new T;
		if (Widget* buf = static_cast<Widget*>(gameWid))
		{
			widgets.insert(buf);
			return gameWid;
		}
		delete gameWid;
		return nullptr;
	}


	void AddToViewport();

	void HideFromViewport();

	void RemoveFromViewport();
};