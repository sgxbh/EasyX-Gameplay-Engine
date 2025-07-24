#pragma once
#include"CoreMinimal.h"
#include"VisualInterface.h"

class SpriteRenderer final:public SceneComponent,public LayerInterface,public ImageInterface {
public:
	virtual void Render()override;
	virtual void Update()override;
	void FlipImage(IMAGE* srcImg, IMAGE* dstImg, bool bIsHorizontal);
};

class StaticMesh :public Object {
	SpriteRenderer* renderer;
public:
	StaticMesh();

	virtual void BeginPlay()override {};
	
};