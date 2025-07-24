#include "SpriteRenderer.h"
#pragma comment(lib,"Msimg32.lib")
#include"Camera.h"


void SpriteRenderer::Render()
{
    if (sprite) {
        Vector2D pos = (GetWorldPosition() - mainWorld.mainCamera->GetCameraPosition()) * 20.f / mainWorld.mainCamera->springArmLength_virtual + Vector2D(WIN_WIDTH / 2, WIN_HEIGHT / 2);
        HDC dstDC = GetImageHDC();
        IMAGE* img = copy ? copy : sprite;

        IMAGE* mirImg{};
        if (GetWorldScale().x < 0) // 检查是否需要水平镜像
        {
            mirImg = new IMAGE(img->getwidth(), img->getheight());
            FlipImage(img, mirImg, true); // 水平翻转
            HDC imgDC = GetImageHDC(mirImg);
            BitBlt(imgDC, 0, 0, mirImg->getwidth(), mirImg->getheight(), imgDC, 0, 0, SRCCOPY);
            img = mirImg;
        }

        HDC srcDC = GetImageHDC(img);
        int w = img->getwidth();
        int h = img->getheight();

        int dw = int(abs(w * GetWorldScale().x * 20.f / mainWorld.mainCamera->springArmLength_virtual));
        int dh = int(abs(h * GetWorldScale().y * 20.f / mainWorld.mainCamera->springArmLength_virtual));

        SetStretchBltMode(dstDC, HALFTONE);
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };

        /*if (filterLayers.size() > 0 && filter) {
            HDC src_filter = GetImageHDC(filter);
            AlphaBlend(dstDC, (int)pos.x - dw / 2, (int)pos.y - dh / 2, dw, dh, src_filter, 0, 0, w, h, bf);
        }
        else {*/
            AlphaBlend(dstDC,
                (int)pos.x - dw / 2,
                (int)pos.y - dh / 2,
                dw, dh,
                srcDC,
                0, 0, w, h,
                bf);
            if (mirImg)delete mirImg;
    }
}

void SpriteRenderer::Update()
{
	if (GetWorldRotation() != angle) {
		angle = GetWorldRotation();
		if (sprite)RotateImage(PI * angle / 180);
	}
	if (sprite && filterLayers.size()>0)FilterImage();
}

void SpriteRenderer::FlipImage(IMAGE* srcImg, IMAGE* dstImg, bool bIsHorizontal)
{
	if (!srcImg || !dstImg)return;

	const DWORD* pBuf = GetImageBuffer(srcImg);
	DWORD* pNewBuf = GetImageBuffer(dstImg);
	int width = srcImg->getwidth(), height = srcImg->getheight();

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			pNewBuf[bIsHorizontal ? ((i + 1) * width - j - 1) : ((height - i - 1) * width + j)] = pBuf[i * width + j];
		}
	}
}

StaticMesh::StaticMesh()
{
	renderer=ConstructComponent<SpriteRenderer>();
	renderer->AttachTo(root);
}
