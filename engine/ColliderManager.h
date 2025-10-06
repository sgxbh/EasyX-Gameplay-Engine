#pragma once
#include <unordered_set>




enum class CollisionType :uint8_t
{
	Default,
	attackbox,
	b,
	player,
	ground,
	WorldStatic
};




class CollisionManager
{
public:
	CollisionManager();

	void Initialize();

	bool FindMapping(CollisionType type1, CollisionType type2);

private:
	std::unordered_set<int>collisionMap;

	void AddMapping(CollisionType type1, CollisionType type2);
};