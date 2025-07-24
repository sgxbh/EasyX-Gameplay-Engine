#pragma once
#include<unordered_map>
#include<easyx.h>
#include<string>

struct AnimationResource {
	IMAGE** images;
	int num;
};

class ResourceManager {
	std::unordered_map<std::string, IMAGE*>ImagePool;
	std::unordered_map<std::string, AnimationResource>animationPool;
	void Load(std::string name, std::string path, int wid, int hei);
	void Load(std::string name, std::string path, int wid, int hei, int num);
	void Load(std::string name, std::string path, int wid, int hei, int num,int row,int col);
public:
	void Initialize();

	IMAGE* Fetch(std::string name);

	AnimationResource FetchAnimation(std::string name);
};