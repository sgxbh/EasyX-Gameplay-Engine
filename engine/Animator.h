#pragma once
#include"CoreMinimal.h"
#include"easyx.h"
#include"Timer.h"
#include<unordered_map>
#include"Delegate.h"

DECLARE_NO_PARAM_UNICAST_DELEGATE_CLASS(AnimationDelegate)


//动画源
class Animation
{
	friend class Player;
	friend class Animator;
	int num = 0;//动画帧数
	int index = 0;//当前索引
	IMAGE** images = nullptr;//动画帧数组
	POINT offset = { 0,0 };//偏移量
	float interval = 1.f;

	Timer clock;

	std::unordered_map<int, AnimationDelegate>notifications;
public:
	Animation() { clock.Bind(1.f, this, &Animation::Tick, true); std::cout << "clock bind" << std::endl; }

	void Tick();

	void Load(std::string name, POINT delta = {0,0});

	void SetInterval(float interval) { clock.SetDelay(interval); this->interval = interval; }
	float Getinterval() { return interval; }

	void SetIndex(int i) { index = i; }

	

	void Initial() { };

	void AddNotification(int index, AnimationDelegate& event) { notifications.insert({ index,event }); }
};

class Animator final:public Component {
	std::unordered_map<std::string, Animation*> animations;

	Animation* aniNode = nullptr;

	class SpriteRenderer* rendererAttached = nullptr;
public:
	virtual void Update()override;

	void Insert(std::string name, Animation& ani);

	Animation* GetNode()const { return aniNode; }

	void SetNode(std::string nodename);

	void SetCalled(bool called);
};

