#pragma once
#include<easyx.h>
#include<string>
#include<set>
#include"CoreMinimal.h"

class LayerInterface
{
	int layer = 0;
public:
	LayerInterface();
	virtual ~LayerInterface();
	void SetLayer(int lay);
	int GetLayer()const;

	virtual void Render()=0;
};


struct SpriteInfo {
	Vector2D offset = Vector2D(0, 0);
	Pair size = Pair(0, 0);
	Pair startLoc = Pair(0, 0);
	Pair endLoc = Pair(0, 0);
};

struct FilterInfo {
	COLORREF color = BLACK;
	int level = 50;
	int layer=0;
};

struct FilterSort {
	bool operator()(const FilterInfo& a, const FilterInfo& b)const {
		return a.layer < b.layer;
	}
};

class ImageInterface {
	friend class Animator;
protected:
	IMAGE* sprite = nullptr;
	SpriteInfo spriteInfo;
	BYTE alpha = 255;

	float angle = 0;
	IMAGE* copy = nullptr;
	void RotateImage(double radian);

	
	IMAGE* filter = nullptr;
	std::set<FilterInfo, FilterSort>filterLayers;
	void FilterImage();


public:
	virtual ~ImageInterface() { if (copy)delete copy; if (filter)delete filter; }
	
	BYTE GetTransprancy()const { return alpha; }
	void SetTransprancy(BYTE transprancy) { alpha = transprancy; }

	void LoadSprite(std::string name);

	void SetFilter(bool enable, COLORREF col = BLACK, int level = 50);

	void AddFilter(FilterInfo filterinfo);
	void RemoveFilter();
	
};