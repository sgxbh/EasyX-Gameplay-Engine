#include "CoreMinimal.h"
#include"Timer.h"
#include"VisualInterface.h"
#include"Controller.h"
#include"Camera.h"
#include"Collider.h"
#include"Math.h"
#include"UserInterface.h"


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
		return parent->GetWorldPosition() + transform.position;
	}
	else {
		if (pOwner)return pOwner->GetWorldPosition();
		else return GetLocalPosition();
	}
}

float SceneComponent::GetWorldRotation() const
{
	if (parent) {
		return parent->GetWorldRotation() + transform.rotation;
	}
	else {
		if (pOwner)return pOwner->GetWorldRotation();
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
		return parent->GetWorldPosition() + GetLocalPosition();
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

	for (auto& obj : GameObjects) {
		obj->Update();
	}

	for (auto& obj : GameObjects_to_add) {
		GameObjects.insert(obj);
		obj->BeginPlay();
	}
	GameObjects_to_add.clear();

	for (auto& obj : GameObjects_to_delete) {
		GameObjects_to_delete.erase(obj);
		delete obj;
	}
	GameObjects_to_delete.clear();

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
	
	FlushBatchDraw();
}

void World::Input()
{
	mainController->PickInfo();
}

void World::ProcessColliderZones()
{
	for (auto& arr_i : ColliderZones)for (auto& arr_j : arr_i)if (!arr_j.empty())
		for (auto& me : arr_j)for (auto& he : arr_j)if (he != me)me->Insert(he);

	for (auto& it : GameColliders)it->Erase();
}

void World::Debug()
{
	for (auto& obj : GameColliders)
		obj->DrawDebugLine();
	for (auto& obj : GameObjects) {
		obj->DrawDebugPosition();
	}
	
	
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
	Characters mycharacters;
	mycharacters.SetCharacters("$0ŷҮ1\n$3aaaaaaaaaaa!");
	mycharacters.PrintCharacters({ 300, 200 }, CharactersPattern::Left);
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
