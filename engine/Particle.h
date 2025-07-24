#pragma once
#include"CoreMinimal.h"
#include"VisualInterface.h"
#include<chrono>
#include<queue>

enum class ParticlePattern :uint8_t {
	Center,
	Line
};


//单个粒子信息
struct ParticleInfo {
	Vector2D offset;//相对于粒子创建器中心位置
	int index = 0;
	Vector2D velocity;
	std::chrono::time_point<std::chrono::steady_clock>lastTime;
	float alpha = 255.f;//透明度
	ParticleInfo() {lastTime = std::chrono::steady_clock::now();}
};


class Particle final :public SceneComponent, public LayerInterface, public ImageInterface {
	std::deque<ParticleInfo>particles;
	int capacity = 0;//粒子容量

	float speed = 1.f;
	float gravity = 98.f;
	float lifeCycle = 1.f;
	bool isLoop = true;//是否循环生产
	float interval = 0.1f;//生产间隔
	float fadingTime = 0;//粒子逐渐消失的时间


	IMAGE** images = nullptr;
	int number = 0;

	ParticlePattern pattern = ParticlePattern::Center;
	Vector2D unitVector = Vector2D(0, 1);//粒子方向单位向量
	/*中心放射状属性*/
	Vector2D radius=Vector2D(0,0);//内外径
	Vector2D scoop = Vector2D(0, 360);//放射范围
	/*线性放射状属性*/
	float length = 1.f;
	float angle = 0.f;


	std::chrono::time_point<std::chrono::steady_clock>lastTime;//记录生产

	void Produce();
public:
	Particle() { lastTime = std::chrono::steady_clock::now(); }
	void Load(std::string name);

	virtual void Update()override;
	virtual void Render()override;

	void SetCapacity(int capacity) { this->capacity = capacity; }
	void SetSpeed(float speed) { this->speed = speed; }
	void SetGravity(float gravity) { this->gravity = gravity; }
	void SetLifeCircle(float lifecircle) { this->lifeCycle = lifecircle; }
	void SetIsLoop(bool isLoop) { this->isLoop = isLoop; }
	void SetInterval(float interval) { this->interval = interval; }
	void SetFadeTime(float fadetime) { this->fadingTime = fadetime; }

	void SetPattern(ParticlePattern pattern) { this->pattern = pattern; }
	void SetCenter(Vector2D radius, Vector2D scoop = Vector2D(0, 360)) { this->radius = radius; this->scoop = scoop; }
	void SetLine(float length, float angle) { this->length = length; this->angle = angle; }

	bool GetIsLoop() { return isLoop; }
};
