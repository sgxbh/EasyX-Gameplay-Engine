#pragma once
#include"CoreMinimal.h"
#include"VisualInterface.h"
#include<chrono>
#include<queue>

enum class ParticlePattern :uint8_t {
	Center,
	Line
};


//����������Ϣ
struct ParticleInfo {
	Vector2D offset;//��������Ӵ���������λ��
	int index = 0;
	Vector2D velocity;
	std::chrono::time_point<std::chrono::steady_clock>lastTime;
	float alpha = 255.f;//͸����
	ParticleInfo() {lastTime = std::chrono::steady_clock::now();}
};


class Particle final :public SceneComponent, public LayerInterface, public ImageInterface {
	std::deque<ParticleInfo>particles;
	int capacity = 0;//��������

	float speed = 1.f;
	float gravity = 98.f;
	float lifeCycle = 1.f;
	bool isLoop = true;//�Ƿ�ѭ������
	float interval = 0.1f;//�������
	float fadingTime = 0;//��������ʧ��ʱ��


	IMAGE** images = nullptr;
	int number = 0;

	ParticlePattern pattern = ParticlePattern::Center;
	Vector2D unitVector = Vector2D(0, 1);//���ӷ���λ����
	/*���ķ���״����*/
	Vector2D radius=Vector2D(0,0);//���⾶
	Vector2D scoop = Vector2D(0, 360);//���䷶Χ
	/*���Է���״����*/
	float length = 1.f;
	float angle = 0.f;


	std::chrono::time_point<std::chrono::steady_clock>lastTime;//��¼����

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
