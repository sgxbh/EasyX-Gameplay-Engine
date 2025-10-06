#include"CoreMinimal.h"
#include<Windows.h>
#pragma comment(lib,"winmm.lib")
#include<easyx.h>
#include"ResourceManager.h"
#include"Level_0.h"
#include"Controller.h"
#include"BattleController.h"
#include"GameStatics.h"
#include"ColliderManager.h"
#include<chrono>




class Engine {
public:
	static void Init() {
		initgraph(WIN_WIDTH, WIN_HEIGHT);

		mainWorld.resourcePool = new ResourceManager;
		mainWorld.resourcePool->Initialize();

		mainWorld.collisionManager = new CollisionManager;
		mainWorld.collisionManager->Initialize();

		BeginBatchDraw();
		
		mainWorld.currentLevel = new Level_0;
		mainWorld.gameInstance = new GameInstance;
		mainWorld.mainController =GameStatics::CreateObject<BattleController>();


		mainWorld.FPSClock = new Timer;
	}

	static void Tick() {
		
		mainWorld.Update();
		mainWorld.Render();
	}


	static void Tick_() {
		mainWorld.Input();
		mainWorld.Update_();
	}



	static void Exit() {

	}
};


void CALLBACK timecallback(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) {
	Engine::Tick();
}

void CALLBACK timecallback_(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) {
	Engine::Tick_();
}

int main() {
	
	
	Engine::Init();
	using clock = std::chrono::high_resolution_clock;
	auto last = clock::now();

	/*timeSetEvent(UINT(DELTA_TIME * 1000), 1, timecallback, NULL, TIME_CALLBACK_FUNCTION | TIME_PERIODIC);
	timeSetEvent(1, 1, timecallback_, NULL, TIME_CALLBACK_FUNCTION | TIME_PERIODIC);*/
	while (true) {
		auto now = clock::now();
		std::chrono::duration<float> delta = now - last;
		last = now;
		
		Engine::Tick();
		Engine::Tick_();

		float frameTime = delta.count(); // √Î
		float targetFrameTime = 1.0f / 120.0f;
		if (frameTime < targetFrameTime) {
			Sleep(DWORD((targetFrameTime - frameTime) * 1000));
		}
	}

	

	return 0;
}