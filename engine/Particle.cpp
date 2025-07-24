#include "Particle.h"
#include"ResourceManager.h"
#include"Math.h"
#include"Camera.h"

void Particle::Produce()
{
	ParticleInfo tem;
	tem.alpha = alpha;
	tem.index = Math::RandomInt(0, number - 1);
	if (pattern == ParticlePattern::Center) {
		double radian=Math::RandomReal(scoop.x, scoop.y)* PI / 180;
		unitVector = Vector2D(cos(radian), sin(radian));
		if (radius != Vector2D(0, 0))tem.offset = unitVector * (std::abs(radius.y - radius.x) * Math::RandomPerc() + min(radius.x,radius.y));
		tem.velocity = unitVector * speed;
	}
	else {
		double radian = angle * PI / 180;
		unitVector= Vector2D(cos(radian), sin(radian));
		Vector2D lineVector = Vector2D::RotateVector(90, unitVector);
		if (length)tem.offset = lineVector * (-length / 2 + length * Math::RandomPerc());
		tem.velocity = unitVector *speed;
	}
	particles.push_back(tem);
}

void Particle::Load(std::string name)
{
	AnimationResource aniRes = mainWorld.resourcePool->FetchAnimation(name);
	number = aniRes.num;
	images = aniRes.images;
}

void Particle::Update()
{
	if (particles.size() < capacity) {

		if (interval == 0)for (int i = 0; i < number; i++)Produce();
		else if (std::chrono::duration<float>(std::chrono::steady_clock::now() - lastTime).count() >= interval &&interval>0)
		{
			Produce();
			lastTime = std::chrono::steady_clock::now();
		}

		if (!isLoop && particles.size() >= capacity)capacity = 0;
	}//生产管理

	if (particles.size()) {
		if (interval == 0) {
			if (std::chrono::duration<float>(std::chrono::steady_clock::now() - particles.begin()->lastTime).count() >= lifeCycle) {
				particles.clear();
			}
			else if (fadingTime && std::chrono::duration<float>(std::chrono::steady_clock::now() - particles.begin()->lastTime).count() >= lifeCycle - fadingTime) {
				for (auto each : particles)each.alpha -= alpha * DELTA_TIME / fadingTime;
			}
		}
		else {
				if (std::chrono::duration<float>(std::chrono::steady_clock::now() - particles.begin()->lastTime).count() >= lifeCycle ) {
					particles.pop_front(); 
				}
			}
		}
	//销毁管理

	for (auto &each : particles) {
		if (gravity)each.velocity.y += gravity * DELTA_TIME;
		each.offset += each.velocity * DELTA_TIME;

		if (fadingTime && std::chrono::duration<float>(std::chrono::steady_clock::now() - each.lastTime).count() >= lifeCycle - fadingTime) {
			each.alpha -= alpha * DELTA_TIME / fadingTime;
		}
	}

}

void Particle::Render()
{
	if (images) {
		Vector2D center = GetWorldPosition() - mainWorld.mainCamera->GetCameraPosition();
		HDC dstDC = GetImageHDC();

		
		
		int w = images[0]->getwidth();
		int h = images[0]->getheight();
		float dw = int(w * GetWorldScale().x * 20.f / mainWorld.mainCamera->springArmLength_virtual);
		float dh = int(h * GetWorldScale().y * 20.f / mainWorld.mainCamera->springArmLength_virtual);

		for (auto& each : particles) {
			BLENDFUNCTION bf = { AC_SRC_OVER,0,each.alpha,AC_SRC_ALPHA };
			Vector2D pos = center+each.offset;
			pos = pos * 20.f / mainWorld.mainCamera->springArmLength_virtual + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2)-Vector2D(dw/2,dh/2);
			HDC srcDC = GetImageHDC(images[each.index]);
			AlphaBlend(dstDC, (int)pos.x , (int)pos.y , dw, dh, srcDC, 0, 0, w, h, bf);
		}

		
	}
	
}
