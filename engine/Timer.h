#pragma once
#include"CoreMinimal.h"
#include<functional>
#include<chrono>



class Timer final{
public:
	Timer() { lastTime = std::chrono::steady_clock::now(); };
	~Timer() { mainWorld.GameTimers.erase(this); };


	template<typename T>
	void Bind(double del, T* obj, void(T::* func)(), bool repeat = false) {
		callback = std::bind(func, obj);
		delay = std::chrono::duration<double>(del);
		lastTime = std::chrono::steady_clock::now();
		bPersistent = repeat;
		mainWorld.GameTimers.insert(this);
	}


	void Execute() {
		//std::cout << lastTime.time_since_epoch().count() << " " << std::chrono::steady_clock::now().time_since_epoch().count()<< std::endl;
		//std::cout << getDelay().count() << " " << std::endl;
		if(GetDelay()<0)lastTime = std::chrono::steady_clock::now();
		if (delay.count()>0 && getDelay().count() >= delay.count()&& isRunning) {
			
			callback();
			if (bPersistent)lastTime = std::chrono::steady_clock::now();
			else delay = std::chrono::duration<double>(0);

		}
	}

	double GetDelay() { return getDelay().count(); }

	void SetDelay(double duration) { delay = std::chrono::duration<double>(duration); }

	void Reset() { lastTime = std::chrono::steady_clock::now(); }

	void Stop() { isRunning = false; stopTime = std::chrono::steady_clock::now();}
	void Continue() { isRunning = true; lastTime += (std::chrono::steady_clock::now() - stopTime);}

private:
	std::function<void()> callback;
	bool bPersistent = false;
	bool isRunning = true;
	std::chrono::duration<double>delay= std::chrono::duration<double>(0);
	std::chrono::time_point<std::chrono::steady_clock>lastTime;
	std::chrono::time_point<std::chrono::steady_clock>stopTime;

	std::chrono::duration<double>getDelay() {
		return std::chrono::steady_clock::now() - lastTime;
	}
};
