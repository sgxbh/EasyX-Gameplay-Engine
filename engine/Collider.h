#pragma once
#include"CoreMinimal.h"
#include<functional>
#include"Delegate.h"
#include"ColliderManager.h"

enum class CollisionMode:uint8_t{None,Trigger,Collision};

class Collider;




struct HitResult
{
	Vector2D ImpactPoint;
	Vector2D ImpactNormal;
	Object* HitObject;
	Component* HitComponent;

	HitResult() :ImpactPoint(0, 0), ImpactNormal(0, 0), HitObject(nullptr), HitComponent(nullptr) {}
	HitResult(const Vector2D& impactPoint, const Vector2D& impactNormal, Object* hitObject, Component* hitComponent)
		:ImpactPoint(impactPoint), ImpactNormal(impactNormal), HitObject(hitObject), HitComponent(hitComponent) {}
};

//碰撞委托

/* Collider* overlapComp, Collider* otherComp, Actor* otherActor */
DECLARE_MULTI_PARAM_MULTICAST_DELEGATE_CLASS(CollisionOverlapDelegate, Collider*, Collider*, Object*)

/* Collider* hitComp, Collider* otherComp, Actor* otherActor, Vector2D normalImpulse, const HitResult& hitResult */
DECLARE_MULTI_PARAM_MULTICAST_DELEGATE_CLASS(CollisionHitDelegate, Collider*, Collider*, Object*, Vector2D, const HitResult&)


class Collider:public SceneComponent
{
	friend class Controller;
	friend class RigidBody;
public:
	enum ColliderShap{Circle,Box};
	Collider() { mainWorld.GameColliders.insert(this); }
	virtual ~Collider() { mainWorld.GameColliders.erase(this); Clear();}
	virtual void Update()override;
	virtual void BeginPlay()override;

	const std::vector<Object*>& GetCollisions(CollisionType type){
		aims.clear();
		if (!collisions.empty()) {
			for (auto it = collisions.begin(); it != collisions.end(); ++it)
				if ((*it)->GetType() == type)aims.push_back((*it)->pOwner);
		}
		return aims;
	}

	int GetLayer()const { return layer; }
	void SetLayer(int layer) { this->layer = layer; }
	const CollisionType GetType() { return type; }
	void SetType(CollisionType type) { this->type = type; }

	void SetCollisionMode(CollisionMode mode) { this->mode = mode; }
	CollisionMode GetCollisionMode() { return mode; }

	ColliderShap GetShape() { return shape; };

	void Clear();
	void Erase();
	void Insert(Collider* another);
	virtual void DrawDebugLine() = 0;
	
	//碰撞事件
	CollisionOverlapDelegate OnComponentBeginOverlap;
	CollisionOverlapDelegate OnComponentEndOverlap;
	CollisionHitDelegate OnComponentHit;

	HitResult CollisionHit(Collider* another);

	//碰撞信息
	static HitResult(*collisionHitMap[3])(Collider*, Collider*);
	static HitResult collisionHitCircleToCircle(Collider* c1, Collider* c2);
	static HitResult collisionHitCircleToBox(Collider* c1, Collider* c2);
	static HitResult collisionHitBoxToBox(Collider* c1, Collider* c2);
protected:
	ColliderShap shape = Circle;
	virtual bool CollisionJudge(Collider* another) ;
	virtual bool IsMouseOver() = 0;
private:
	CollisionType type;
	int layer = 0;
	CollisionMode mode = CollisionMode::Trigger;

	std::unordered_set<Collider*>collisions;
	std::vector<Object*>aims;
	std::vector<Collider*>collisions_to_erase;

	Pair point{ -1,-1 }, point_1{ -1,-1 };

	class RigidBody* rigidAttached = nullptr;

	static bool (*collisionJudgeMap[3])(Collider*, Collider*);
	static bool collisionJudgeCircleToCircle(Collider* c1, Collider* c2);
	static bool collisionJudgeCircleToBox(Collider* c1, Collider* c2);
	static bool collisionJudgeBoxToBox(Collider* c1, Collider* c2);
};


class CircleCollider final :public Collider {
	float radius = 0;
	float radius_ini = 0;
	
public:
	CircleCollider() { shape = Circle; }
	virtual void Update()override;
	virtual void DrawDebugLine()override;
	float GetRadius()const { return radius; }
	void SetRadius(float r) { radius = r; radius_ini = r / sqrtf(GetWorldScale().x * GetWorldScale().y); }

	virtual bool IsMouseOver() override;
};

class BoxCollider final :public Collider
{
public:
	struct Rect
	{
		float left = 0, top = 0, right = 0, bottom = 0;
	};

	BoxCollider() { shape = Box; }
	virtual void Update()override;
	virtual void DrawDebugLine()override;
	virtual bool IsMouseOver()override;
	Vector2D GetSize()const { return size; }
	void SetSize(Vector2D size) { this->size = size; size_ini = size / GetWorldScale(); }

	Rect GetRect()const { return rect; }

	//获取重叠矩形宽高，需传入已经确定发生碰撞的两个碰撞器
	static Vector2D GetOverlapRect(const Rect& r1, const Rect& r2);
private:
	Vector2D size = Vector2D(0, 0);
	Vector2D size_ini = Vector2D(0, 0);
	Rect rect;
};

