#include "VisualInterface.h"
#include"CoreMinimal.h"
#include"ResourceManager.h"
#include"Math.h"

LayerInterface::LayerInterface()
{
	mainWorld.GameRenderers.insert(this);
}

LayerInterface::~LayerInterface()
{
	mainWorld.GameRenderers.erase(this);
}

void LayerInterface::SetLayer(int lay)
{
	mainWorld.GameRenderers.erase(this);
	layer = lay;
	mainWorld.GameRenderers.insert(this);
}

int LayerInterface::GetLayer()const
{
	return layer;
}

void ImageInterface::LoadSprite(std::string name)
{
	sprite = mainWorld.resourcePool->Fetch(name);
}

void ImageInterface::SetFilter(bool enable, COLORREF col, int level)
{
	filterLayers.erase(FilterInfo{ col,level });
	if (enable) {
		level = Math::Clamp(level, 0, 100);
		filterLayers.insert(FilterInfo{ col,level });
	}
}

void ImageInterface::AddFilter(FilterInfo filterinfo)
{
	filterLayers.insert(filterinfo);
}

void ImageInterface::RemoveFilter()
{
	filterLayers.erase({ BLACK,60,1 });
}

void ImageInterface::RotateImage(double radian)
{
	float fSin = (float)sin(radian), fCos = (float)cos(radian);
	int w = sprite->getwidth(), h = sprite->getheight();
	POINT point[4] = { {0,0},{w,0},{0,h},{w,h} };
	int min_x = 0, min_y = 0;
	int max_x = 0, max_y = 0;
	for (int i = 0; i < 4; i++) {
		point[i] = {
			(int)(point[i].x * fCos - point[i].y * fSin),
			(int)(point[i].x * fSin + point[i].y * fCos)
		};
		min_x = min(min_x, point[i].x);
		min_y = min(min_y, point[i].y);
		max_x = max(max_x, point[i].x);
		max_y = max(max_y, point[i].y);
	}

	int nw = max_x - min_x;
	int nh = max_y - min_y;
	int nSize = nw * nh;

	if (!copy)copy = new IMAGE(nw, nh);
	copy->Resize(nw, nh);

	DWORD* pNewBuf = GetImageBuffer(copy);
	DWORD* pBuf = GetImageBuffer(sprite);

	for (int i = min_x,ni=0; ni < nw; i++,++ni) {
		for (int j = min_y,nj=0; nj < nh; j++,++nj) {

			pNewBuf[nj * nw + ni] = 0;
			int nx = (int)(i * fCos + j * fSin);
			int ny = (int)(-i * fSin + j * fCos);
			if (nx >= 0 && nx < w && ny >= 0 && ny < h)
				pNewBuf[nj * nw + ni] = pBuf[ny * w + nx];
		}
	}
}

void ImageInterface::FilterImage()
{
	IMAGE* img = copy ? copy : sprite;
	if (!filter)filter = new IMAGE(img->getwidth(), img->getheight());
	filter->Resize(img->getwidth(), img->getheight());

	const DWORD* pBuf = GetImageBuffer(img);
	DWORD* pNewBuf= GetImageBuffer(filter);
	int num = img->getheight() * img->getwidth();

	for (int i = 0; i < num; ++i) {
		
		if (pBuf[i] >> 24) {
			DWORD pNewBufB = pBuf[i] & 0xFF;
			DWORD pNewBufG = (pBuf[i] & 0xFF00) >> 8;
			DWORD pNewBufR = (pBuf[i] & 0xFF0000) >> 16;

			for (auto& filterinfo : filterLayers) {
				int level = filterinfo.level;
				if (pBuf[i] >> 24 < 250)level = (pBuf[i] >> 24) * level >> 8;
				pNewBufB = (pNewBufB * (128 - level) + level * GetBValue(filterinfo.color)) >> 7;
				pNewBufG = (pNewBufG * (128 - level) + level * GetGValue(filterinfo.color)) >> 7;
				pNewBufR = (pNewBufR * (128 - level) + level * GetRValue(filterinfo.color)) >> 7;
			}

			pNewBuf[i] = (pBuf[i] & 0xFF000000) | (pNewBufR) << 16 | (pNewBufG) << 8 | pNewBufB;
		}
		else pNewBuf[i] = 0;
	}
}
