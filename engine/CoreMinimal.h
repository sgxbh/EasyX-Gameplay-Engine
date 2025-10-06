#pragma once
#include<math.h>
#include<iostream>
#include<set>
#include<unordered_set>
#include<vector>
#include<stack>
#include<string>
#include<stdint.h>



#define PI 3.1415926
#define DELTA_TIME 0.01
#define WIN_WIDTH 1600
#define WIN_HEIGHT 900

struct Vector2D {
	float x;
	float y;
	Vector2D():x(0),y(0){}
	Vector2D(float a,float b):x(a),y(b){}

	static float Distance(const Vector2D& a, const Vector2D& b) {
		return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	float Size()const {
		return sqrtf(x * x + y * y);
	}

	static Vector2D ProjectVector(const Vector2D& u, const Vector2D& v) {
		float scaler = DotProduct(u, v) / v.Size();
		return v.Normalize() * scaler;
	}

	Vector2D Normalize()const {
		return Vector2D(x / Size(), y / Size());
	}

	static Vector2D RotateVector(float angle, const Vector2D& another) {
		double radian = PI * angle / 180;
		float fsin = float(sin(radian));
		float fcos = float(cos(radian));
		return Vector2D(another.x * fcos - another.y * fsin, another.x * fsin + another.y * fcos);
	}

	std::string ToString()const {
		return "(" + std::to_string((int)x) + "," + std::to_string((int)y) + ")";
	}

	static float DotProduct(const Vector2D& v1, const Vector2D& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}


	Vector2D operator+(const Vector2D& another) {
		return Vector2D(x + another.x, y + another.y);
	}

	Vector2D operator-(const Vector2D& another) {
		return Vector2D(x - another.x, y - another.y);
	}

	Vector2D operator-() const {
		return Vector2D(-x, -y);
	}

	Vector2D operator+=(const Vector2D& another) {
		x += another.x; y += another.y;
		return *this;
	}

	Vector2D operator-=(const Vector2D& another) {
		x -= another.x; y -= another.y;
		return *this;
	}

	Vector2D operator*(const Vector2D& another) {
		return Vector2D(x * another.x, y * another.y);
	}
	Vector2D operator*(float scalar) const {
		return Vector2D(x * scalar, y * scalar);
	}

	Vector2D operator/(const Vector2D& another) {
		return Vector2D(x / another.x, y / another.y);
	}
	Vector2D operator/(float scalar) {
		return Vector2D(x /scalar, y /scalar);
	}

	bool operator==(const Vector2D& another) {
		return (x==another.x && y==another.y);
	}

	Vector2D& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	bool operator!=(const Vector2D& another) {
		return (x != another.x || y != another.y);
	}

};

static std::ostream& operator<<(std::ostream & out, const Vector2D & another) {
	std::cout << "(" << another.x << "," << another.y << ")";
	return out;
}


class Base {
public:
	virtual void Update() = 0;
	virtual void BeginPlay() {};
};


class Component : public Base {
protected:	
	class Object* pOwner = nullptr;
	
public:
	virtual ~Component() {};
	
	virtual void Update() override {};

	virtual void BeginPlay() {};

	void SetOwner(class Object* owner) { pOwner = owner; };

	virtual void Destruct();
};


struct Transform {
	Vector2D position;
	float rotation;
	Vector2D scale;


	Transform():position(0,0),rotation(0),scale(1,1){}
	Transform(const Vector2D& pos,float rot,const Vector2D scale):
		position(pos),rotation(rot),scale(scale){}


	Transform Indentity()const {
		return Transform(Vector2D(0, 0), 0, Vector2D(1, 1));
	}
};


struct Pair {
	int x, y;
	Pair():x(0),y(0){}
	Pair(int x,int y):x(x),y(y){}
	bool operator==(const Pair& another) {
		return x == another.x && y == another.y;
	}
	bool operator!=(const Pair& another) {
		return x != another.x || y != another.y;
	}
};



class SceneComponent :public Component {

	Transform transform;
	std::unordered_set<SceneComponent*>children;
	SceneComponent* parent = nullptr;
	void process_Destruct();
	

public:
	virtual ~SceneComponent() {
		
	}
	
	virtual void Update()override {};

	void AttachTo(SceneComponent* par);
	void DetachFrom(SceneComponent* par);

	virtual void Destruct()override;
	


	Vector2D GetLocalPosition() const { return transform.position; }
	float GetLocalRotation() const {return transform.rotation;}
	Vector2D GetLocalScale() const { return transform.scale; }

	Vector2D GetWorldPosition()const;
	float GetWorldRotation() const;
	Vector2D GetWorldScale() const;

	virtual void SetLocalPosition(const Vector2D& pos) { transform.position = pos; }
	void SetLocalRotation(float rot) { transform.rotation = rot; }
	void SetLocalScale(const Vector2D& scale) { transform.scale = scale; };

	virtual void AddPosition(const Vector2D& pos) { transform.position += pos; }
	virtual void AddRotation(float rot) { transform.rotation += rot; }
};


class Object :public Base {
	friend class Level_0;
	std::unordered_set<Object*>children;
	Object* parent = nullptr;
	std::unordered_set<Component*>components;
	std::unordered_set<Component*>::iterator components_iter;
	
protected:
	
	
public:
	Object() { root->SetOwner(this); }
	SceneComponent* const root = new SceneComponent;
	
	virtual ~Object() {
		for (Component* com : components)
		{
			delete com;
		}
		delete root;
	}

	virtual void Update()override {
		components_iter = components.begin();
		while (components_iter != components.end()) {

			(*components_iter)->Update();
			if (components_iter == components.end())break;
			components_iter++;
		}
	}

	virtual void BeginPlay() override;

	template<typename T>
	T* GetComponentByClass() {
		for (auto& com : components) {
			if (T* pCom = dynamic_cast<T*>(com)) return pCom;
		}
		return nullptr;
	}


	void RegisterComponent(Component* pCom) {
		components.insert(pCom);
	}


	void UnregisterComponent(Component* pCom) {
		auto it = components.find(pCom);
		components_iter = components.erase(it);
	}
	
	
	void AttachTo(Object* par);
	void DetachFrom(Object* par);


	template<typename T>
	T* ConstructComponent() {
		T* pCom = new T();
		if (pCom && static_cast<Component*>(pCom)) {
			pCom->SetOwner(this);
			RegisterComponent(pCom);
			return pCom;
		}

		return nullptr;
	}

	void Destroy();
	
	Vector2D GetLocalPosition() const{ return root->GetLocalPosition(); }
	float GetLocalRotation() const{ return root->GetLocalRotation(); }
	Vector2D GetLocalScale()const { return root->GetLocalScale(); }

	Vector2D GetWorldPosition()const;
	float GetWorldRotation() const;
	Vector2D GetWorldScale() const;

	void SetLocalPosition(const Vector2D& pos)  { root->SetLocalPosition(pos); }
	void SetLocalRotation(float rot)  { root->SetLocalRotation(rot); }
	void SetLocalScale(const Vector2D& scale) { root->SetLocalScale(scale); }

	void AddPosition(const Vector2D& pos) { root->AddPosition(pos); }
	void AddRotation(float rot) { root->AddRotation(rot); }

	void DrawDebugPosition();
};







class GameInstance {

};

class Level :public Base {
public:
	virtual void Update()override;
};


struct LayerSort {
	bool operator()(const class LayerInterface* a, const class LayerInterface* b)const;
};

struct ColliderSort {
	bool operator()(const class Collider* a, const class Collider* b)const;
};

template<typename T>
class QuadTree;

class World{
	friend class Engine;
	friend class Timer;
	friend class GameStatics;
	friend class LayerInterface;
	friend class ImageInterface;
	friend class Camera;
	friend class Animation;
	friend class Particle;
	friend class SpriteRenderer;
	friend class Collider;
	friend class CircleCollider;
	friend class BoxCollider;
	friend class Controller;
	friend class Object;
	friend class Widget;
	friend class UserInterface;
	friend class Button;
	friend class RigidBody;
	friend class QuadTree<class Collider>;
	friend void Object::Destroy();
	
	/*资源管理器*/
	class ResourceManager* resourcePool = nullptr;

	/*碰撞管理器*/
	class CollisionManager* collisionManager = nullptr;

	/*全局计时器*/
	class Timer* FPSClock;
	
	/*场景对象、UI、计时容器*/
	std::unordered_set<Object*>GameObjects;
	std::unordered_set<class RigidBody*>RigidBodies;
	std::vector<Object*>GameObjects_to_add;
	std::unordered_set<Object*>GameObjects_to_delete;
	std::unordered_set<class UserInterface*>GameUIs;
	std::unordered_set<class UserInterface*>GameUIs_to_delete;
	std::unordered_set<class Timer*>GameTimers;

	/*渲染、碰撞计算容器*/
	std::set<class LayerInterface*,LayerSort>GameRenderers;
	std::unordered_set<class Collider*>GameColliders;
	std::set<class Collider*, ColliderSort>ColliderZones[16][9];
	QuadTree<class Collider>* TreeRoot;
	std::set<class LayerInterface*, LayerSort>UIDetectZones[16][9];

	/*游戏单例对象*/
	Level* currentLevel = nullptr;
	class Controller* mainController = nullptr;
	GameInstance* gameInstance = nullptr;
	class Camera* mainCamera = nullptr;

	/*核心逻辑*/
	void Update();

	void Update_();

	void Render();

	void Input();

	void ProcessColliderZones();

	void ColliderInTree(QuadTree<class Collider>* root);
	
	void Debug();
};


extern World mainWorld;



//template<typename T>
//inline T* Cast(Base* pBase) {
//	if(pBase)return dynamic_cast<T*>(pBase);
//	return nullptr;
//}

template<typename T, typename F>
inline T* Cast(F* pBase)
{
	if (pBase)return dynamic_cast<T*>(pBase);

#ifdef _DEBUG
	std::cout << "Cast Error!" << std::endl;
#endif
	return nullptr;
}

