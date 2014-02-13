#pragma once
#include "FpsCounter.h"

class Game
{
public:
	Game();
	~Game();
	void OnUpdate();
	void OnRender(ID2D1HwndRenderTarget* pRenderTarget);
	
	HRESULT CreateDeviceResources(ID2D1Factory *pDirect2dFactory,ID2D1HwndRenderTarget* pRenderTarget);
	void DiscardDeviceResources();

private:
	int m_time;
	FpsCounter m_fps;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
	IDWriteFactory *m_pWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};