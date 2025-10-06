#include "CoreMinimal.h"
#include"Timer.h"
#include"VisualInterface.h"
#include"Controller.h"
#include"Camera.h"
#include"Collider.h"
#include "RigidBody.h"
#include"Math.h"
#include"UserInterface.h"
#include"QuadTree.h"



template<>
std::atomic<size_t> QuadTree<Collider>::allocCount{ 0 };

template<>
std::atomic<size_t> VectorPool<Collider>::Wrapper::allocCount{ 0 };

World mainWorld{};




void Component::Destruct()
{
	pOwner->UnregisterComponent(this);
	delete this;
}

void SceneComponent::process_Destruct()
{
	if (!children.empty()) {
		for (auto& child : children)child->process_Destruct();
	}
	Component::Destruct();
}

Vector2D SceneComponent::GetWorldPosition() const
{
	if (parent) {
		
		return parent->GetWorldPosition() + Vector2D::RotateVector(parent->GetWorldRotation() + GetLocalRotation(), transform.position );
	}
	else {
		if (pOwner) {
			if (pOwner->root != this) { return pOwner->GetWorldPosition() + GetLocalPosition(); }
			else { return  pOwner->GetWorldPosition(); }
		}
		else return GetLocalPosition();
	}
}

float SceneComponent::GetWorldRotation() const
{
	if (parent) {
		return parent->GetWorldRotation() + transform.rotation;
	}
	else {
		if (pOwner) {
			if (pOwner->root != this) { return pOwner->GetWorldRotation() + GetLocalRotation(); }
			else { return  pOwner->GetWorldRotation(); }
		}
		else return GetLocalRotation();
	}
}

Vector2D SceneComponent::GetWorldScale() const
{
	if (parent) {
		return parent->GetWorldScale() * transform.scale;
	}
	else {
		if (pOwner)return pOwner->GetWorldScale();
		else return GetLocalScale();
	}
}

void SceneComponent::AttachTo(SceneComponent* par)
{
	if (!par)return;
	parent = par;
	par->children.insert(this);
	SetOwner(par->pOwner);
	
}

void SceneComponent::DetachFrom(SceneComponent* par)
{
	par->children.erase(this);
	parent = nullptr; 
}

void SceneComponent::Destruct()
{
	if (parent)parent->children.erase(this);

	process_Destruct();
}

void Object::BeginPlay()
{
	for (auto& it : components) {
		it->BeginPlay();
	}
}

void Object::AttachTo(Object* par)
{
	parent = par;
	par->children.insert(this);
}

void Object::DetachFrom(Object* par)
{
	par->children.erase(this);
	parent = nullptr;
}

void Object::Destroy()
{
	if (parent)parent->children.erase(this);

	std::stack<Object*>objects_to_delete;
	objects_to_delete.push(this);
	while (!objects_to_delete.empty()) {
		Object* current_obj = objects_to_delete.top();
		objects_to_delete.pop();
		if (!current_obj->children.empty()) {
			for (auto& child : current_obj->children) {
				objects_to_delete.push(child);
			}
		}
		mainWorld.GameObjects_to_delete.insert(current_obj);
	}

	
}

Vector2D Object::GetWorldPosition() const
{
	if (parent) {
		return parent->GetWorldPosition() + Vector2D::RotateVector(parent->GetWorldRotation() + GetLocalRotation(), GetLocalPosition());
	}
	else return GetLocalPosition();
	
}

float Object::GetWorldRotation() const
{
	if (parent) {
		return parent->GetWorldRotation() + GetLocalRotation();
	}
	else return GetLocalRotation();
}

Vector2D Object::GetWorldScale() const
{
	if (parent) {
		return parent->GetWorldScale() * GetLocalScale();
	}
	else return GetLocalScale();
}

void Object::DrawDebugPosition()
{
	Vector2D pos=(GetWorldPosition()-mainWorld.mainCamera->GetCameraPosition())* 20.f / mainWorld.mainCamera->springArmLength_virtual + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2);
	outtextxy((int)pos.x, (int)pos.y, GetWorldPosition().ToString().c_str());
}

void World::Update()
{
	
	ProcessColliderZones();

	for (Object* obj : GameObjects) {
		obj->Update();
	}

	for (auto& rig : RigidBodies)
	{
		rig->ApplyPendingVelocity();
	}

	

	for (auto& obj : GameUIs) {
		obj->Update();
	}

	for (auto& obj : GameUIs_to_delete) {
		GameUIs.erase(obj);
		delete obj;
	}
	GameUIs_to_delete.clear();

	for (auto& obj : GameTimers) {
		obj->Execute();
	}

	currentLevel->Update();

	for (auto& obj : GameObjects_to_add) {
		GameObjects.insert(obj);
		obj->BeginPlay();
	}
	GameObjects_to_add.clear();

	for (auto& obj : GameObjects_to_delete) {

		if (obj) {
			GameObjects.erase(obj);
			delete obj;
		}
		
	}
	GameObjects_to_delete.clear();

	
	std::cout << "VectorPool<Collider>::Wrapper allocCount: " << VectorPool<Collider>::Wrapper::allocCount.load() << std::endl;

	
	
	/*VectorPool<Collider>::Wrapper::allocCount=0;*/
}

void World::Update_()
{
	mainCamera->Calculate();
}

void World::Render()
{
	cleardevice();
	for (auto& obj : GameRenderers) {
		obj->Render();
	}
	for (auto& obj : GameUIs)obj->ShowInfoBox();
	Debug();
	outtextxy(100, 0, "get");
	
	FlushBatchDraw();
}

void World::Input()
{
	mainController->PickInfo();
}

void World::ProcessColliderZones()
{
	if (!TreeRoot)TreeRoot = QuadTree<Collider>::CreateRoot();
	
	for (auto& it : GameColliders)it->Erase();
	

	TreeRoot->Clear();
	int top = INT_MAX;
	int bottom = INT_MIN;
	int left = INT_MAX;
	int right = INT_MIN;

	for (auto& col : GameColliders)
	{
		col->ReSetRect();
		int colLeft = int(col->GetWorldPosition().x - col->Rwidth / 2);
		int colRight = int(col->GetWorldPosition().x + col->Rwidth / 2);
		int colBottom = int(col->GetWorldPosition().y + col->Rheight / 2);
		int colTop = int(col->GetWorldPosition().y - col->Rheight / 2);

		if (colLeft < left) left = colLeft;
		if (colRight > right) right = colRight;
		if (colBottom > bottom) bottom = colBottom;
		if (colTop < top) top = colTop;
	}

	Vector2D rootCenter;
	rootCenter.x = (left + right) / 2.0f;
	rootCenter.y = (bottom + top) / 2.0f;

	int width = right - left;
	int height = bottom-top;
	int rootSize = max(width, height);

	// 设置根节点
	TreeRoot->InitRect(rootCenter, rootSize);

	for(auto& col : GameColliders)
	{
		TreeRoot->Insert(col);
	}
	ColliderInTree(TreeRoot);
}

void World::ColliderInTree(QuadTree<class Collider>* root)
{
	if (root->IsLeaf)
	{
		for (Collider* me : root->objects->vec)
		{
			for (Collider* he : root->objects->vec)
			{
				if (he != me)me->Insert(he);
			}
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			ColliderInTree(root->children[i]);
		}
	}
}

void World::Debug()
{
	for (auto& obj : GameColliders)
		if(obj)obj->DrawDebugLine();
		
	for (auto& obj : GameObjects) {
		if (obj)obj->DrawDebugPosition();
	}
	if (TreeRoot)TreeRoot->DrawDebug();
	
	
	static int FPS = 0;
	static int num = 0;
	num++;
	setbkmode(TRANSPARENT);
	outtextxy(0, 0, (std::string("FPS:") + std::to_string(FPS)).c_str());
	if (FPSClock->GetDelay() >= 1.0) {
		FPS = num;
		FPSClock->Reset();
		num = 0;
	}
	/*Characters mycharacters;
	mycharacters.SetCharacters("$0ŷҮ1\n$3aaaaaaaaaaa!");
	mycharacters.PrintCharacters({ 300, 200 }, CharactersPattern::Left);*/
}

void Level::Update()
{
}

bool LayerSort::operator()(const LayerInterface* a, const LayerInterface* b) const
{
	if (a->GetLayer() == b->GetLayer()) {
		return a < b;
	}
	else {
		return a->GetLayer() < b->GetLayer();
	}
}

bool ColliderSort::operator()(const Collider* a, const Collider* b) const
{
	if (a->GetLayer() == b->GetLayer())return a < b;
	else return a->GetLayer() < b->GetLayer();
}
