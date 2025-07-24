#include "Camera.h"
#include"Math.h"

Vector2D Camera::Lerp(Vector2D start, Vector2D end, float t)
{
	t = Math::Clamp(t, 0.001f, 0.1f);
	return start + (end - start) * t;
}

float Camera::Lerp(float start, float end, float t)
{
	t = Math::Clamp(t, 0.001f, 0.1f);
	return start + (end - start) * t;
}

float Camera::SmoothStep(float x)
{
	x = x < 1 ? 1 : x;
	return pow(x, float(smoothness) / 1000);
}

void Camera::Update()
{
	
}

void Camera::SetSmoothness(short smooth)
{
	this->smoothness = Math::Clamp(smooth, 0.0, 100.0);
}

void Camera::SetDistanceThreshold(float threshold)
{
	this->distanceThreshold = Math::Clamp(threshold, 0.0, 500.0);
}

void Camera::SetSpringArmLength(float length)
{
	this->springArmLength = Math::Clamp(length, 1.f, 10000.f);
}

void Camera::SetSpringArmSmooth(int smooth)
{
	this->smoothnessForSpring = Math::Clamp(smooth, 0, 100);
}

void Camera::ShakeCamera(char intensity,int decay)
{
	this->shakeDecay = Math::Clamp(decay, 1.0, 100.0);
	this->intensity = intensity;
	if (!shakeFlag)shakeCenter = GetLocalPosition();
	shakeFlag = true;
	
}

void Camera::Calculate()
{
	static bool first = true;
	if (first) {
		transform_vitual.position = GetWorldPosition();
		transform_vitual.rotation = GetWorldRotation();
		springArmLength_virtual = springArmLength;
		first = false;
	}
	
	if (smoothness) {
		transform_vitual.position = Lerp(transform_vitual.position, GetWorldPosition(), 0.1 / smoothness * SmoothStep(Vector2D::Distance(transform_vitual.position, GetWorldPosition()) / distanceThreshold));
	}
	else transform_vitual.position = GetWorldPosition();


	if (smoothnessForSpring) {
		springArmLength_virtual = Lerp(springArmLength_virtual, springArmLength, 0.1 / smoothnessForSpring);
	}
	else springArmLength_virtual = springArmLength;


	if (shakeFlag) {
		if (GetLocalPosition() == shakeCenter) {
			shakeSpeed.x = float(Math::RandomInt(-1, 1));
			shakeSpeed.y = float(Math::RandomInt(-1, 1));
			intensity -= float(shakeDecay) / 100;

			if (intensity <= 0) { shakeFlag = false; return; }
		}
		if (Vector2D::Distance(GetLocalPosition(), shakeCenter) < intensity) {
			AddPosition(shakeSpeed); transform_vitual.position += shakeSpeed;
		}
		else {
			shakeSpeed = -shakeSpeed;
			AddPosition(shakeSpeed);
			transform_vitual.position += shakeSpeed;
		}
		
	}
}
