#pragma once
#include"CoreMinimal.h"


#define min(a,b) ((a)<(b)? (a):(b))
#define max(a,b) ((a)>(b)? (a):(b))

class Camera:public SceneComponent
{
	friend class Object;
	friend class SpriteRenderer;
	friend class CircleCollider;
	friend class BoxCollider;
	friend class Particle;
	
	Transform transform_vitual;

	float distanceThreshold = 100.0f;//æ‡¿Î„–÷µ
	short smoothness = 30;

	char intensity = 0.f;
	bool shakeFlag = false;
	Vector2D shakeCenter = Vector2D();
	Vector2D shakeSpeed = Vector2D(0.05f, 0.05f);
	int shakeDecay = 5;

	float springArmLength = 20.f;
	float springArmLength_virtual = 100.f;
	float smoothnessForSpring = 20.f;

	Vector2D Lerp(Vector2D start, Vector2D end, float t);
	float Lerp(float start, float end, float t);

	float SmoothStep(float x);
public:
	Camera() {};
	~Camera() {};

	virtual void Update()override;

	void SetMainCamera() {
		mainWorld.mainCamera = this;
	}

	Vector2D GetCameraPosition() {
		return transform_vitual.position;
	}

	void SetSmoothness(short smooth);

	void SetDistanceThreshold(float threshold);

	void SetSpringArmLength(float length);

	void SetSpringArmSmooth(int smooth);

	void ShakeCamera(char intensity,int decay);

	void Calculate();

	
};

