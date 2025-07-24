#include "Animator.h"
#include"ResourceManager.h"
#include"SpriteRenderer.h"

void Animation::Tick()
{
	index = (index + 1) % num;
	if (notifications.find(index) != notifications.end())notifications[index].Execute();
}

void Animation::Load(std::string name, POINT delta)
{
	AnimationResource aniRes = mainWorld.resourcePool->FetchAnimation(name);
	num = aniRes.num;
	images = aniRes.images;
	offset = delta;
}

void Animator::Update()
{
	if (!rendererAttached) {
		rendererAttached = pOwner->GetComponentByClass<SpriteRenderer>();
	}
	if (aniNode) {
		IMAGE* sprite= aniNode->images[aniNode->index];
		rendererAttached->sprite = sprite;
		rendererAttached->spriteInfo.offset = Vector2D(aniNode->offset.x, aniNode->offset.y);
		rendererAttached->spriteInfo.endLoc = { sprite->getwidth(),sprite->getheight() };
	}
}

void Animator::Insert(std::string name, Animation& ani)
{
	if(ani.images) animations.insert({ name,&ani });
}

void Animator::SetNode(std::string nodename)
{
	if (aniNode)aniNode->clock.Stop();
	aniNode = animations.find(nodename)->second;
	aniNode->SetIndex(0);
	aniNode->clock.Continue();
	std::cout << "setnode" << std::endl;
}

void Animator::SetCalled(bool called)
{
	called ? aniNode->clock.Continue() : aniNode->clock.Stop();
}
