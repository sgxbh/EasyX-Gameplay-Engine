#include "ColliderManager.h"
#include<iostream>

void CollisionManager::Initialize()
{
	
	AddMapping(CollisionType::attackbox, CollisionType::b);
	AddMapping(CollisionType::player, CollisionType::ground);
	AddMapping(CollisionType::ground, CollisionType::b);
	AddMapping(CollisionType::player, CollisionType::player);
	AddMapping(CollisionType::b, CollisionType::WorldStatic);
	AddMapping(CollisionType::player, CollisionType::WorldStatic);
	
}





CollisionManager::CollisionManager()
{
	
}


bool CollisionManager::FindMapping(CollisionType type1, CollisionType type2)
{
	int result = int(type1) * int(type1) + int(type2) * int(type2);
	return collisionMap.find(result) != collisionMap.end();
}

void CollisionManager::AddMapping(CollisionType type1, CollisionType type2)
{
	int result = int(type1) * int(type1) + int(type2) * int(type2);
	collisionMap.insert(result);
}