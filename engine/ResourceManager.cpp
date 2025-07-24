#include "ResourceManager.h"
#include"iostream"

void ResourceManager::Load(std::string name, std::string path, int wid, int hei)
{
	IMAGE* img = new IMAGE;
	loadimage(img, LPCTSTR(path.c_str()), wid, hei);
	ImagePool.insert({ name,img });
}

void ResourceManager::Load(std::string name, std::string path, int wid, int hei, int num)
{
	
	IMAGE** images = new IMAGE * [num];
	for (int i = 0; i < num; i++) {
		images[i] = new IMAGE;
		loadimage(images[i], LPCTSTR((path + "/" + std::to_string(i) + ".png").c_str()), wid, hei);

	}
	animationPool.insert({ name, {images, num} });
	std::cout << "animation load" << std::endl;
	
}

void ResourceManager::Load(std::string name, std::string path, int wid, int hei, int num, int row, int col)
{
	IMAGE** images = new IMAGE * [num];
	IMAGE img ;
	loadimage(&img, LPCTSTR(path.c_str()), wid, hei);
	DWORD* src = GetImageBuffer(&img);

	int width = wid / col;
	int height = hei / row;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int x = j * width;
			int y = i * height;
			int index = i * col + j;
			if (index >= num)break;
			images[index] = new IMAGE(width, height);
			DWORD* dst = GetImageBuffer(images[index]);

			for (int x1 = 0; x1 < width; ++x1)
				for (int y1 = 0; y1 < height; ++y1)
					dst[x1 + y1 * width] = src[x + x1 + (y + y1) * wid];
		}
	}
	animationPool.insert({ name,{images,num} });
}

void ResourceManager::Initialize()
{
	Load("bg0", "res/0.png", 1920, 1260);
	Load("ground", "res/Floor_1.png", 1177, 220);
	Load("ply", "res/Animation/Player",139,164,6 );
	Load("particle1", "res/wispy_flame_particle_o.png", 204, 1020, 5,5,1);
	Load("player_runR", "res/Animation/Player/Rush", 133, 130, 8);
	Load("player_fall", "res/Animation/Player/Fall", 133, 150, 4);
	Load("player_jumpR", "res/Animation/Player/Jump", 124, 160, 6);
	Load("walk_or_land", "res/spatter_white.png", 31, 31, 1, 1, 1);
}

IMAGE* ResourceManager::Fetch(std::string name)
{
	return (ImagePool.find(name))->second;
}

AnimationResource ResourceManager::FetchAnimation(std::string name)
{
	return (animationPool.find(name))->second;
}
