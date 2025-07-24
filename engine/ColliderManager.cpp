#include "ColliderManager.h"
#include<iostream>

void CollisionManager::Initialize()
{
	
	AddMapping(CollisionType::attackbox, CollisionType::b);
	AddMapping(CollisionType::player, CollisionType::ground);
	
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