#include "stdafx.h"
#include "Game.h"

Game::Game():
	m_time(0),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pWriteFactory(NULL),
	m_pTextFormat(NULL)
{

}

Game::~Game()
{
	DiscardDeviceResources();
}

void Game::OnUpdate()
{
	m_fps.IncLogicFrame();
	m_time++;
	return;
}


//因为将会重复调用此方法，所以可添加一个 if 语句，来检查呈现器目标 (pRenderTarget) 是否已存在。下面的代码演示完整的 CreateDeviceResources 方法。
HRESULT Game::CreateDeviceResources(ID2D1Factory *pDirect2dFactory,ID2D1HwndRenderTarget* pRenderTarget)
{
	HRESULT hr = S_OK;

	if (SUCCEEDED(hr))
	{
		// Create a gray brush.
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::LightSlateGray),
			&m_pLightSlateGrayBrush
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create a blue brush.
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
			&m_pCornflowerBlueBrush
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pWriteFactory->CreateTextFormat(
			L"Gabriola",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			32.0f,
			L"en-us",
			&m_pTextFormat
			);
	}

	return hr;
}

void Game::DiscardDeviceResources()
{
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
	SafeRelease(&m_pWriteFactory);
	SafeRelease(&m_pTextFormat);
}

void Game::OnRender(ID2D1HwndRenderTarget* pRenderTarget)
{

	m_fps.IncRenderFrame();

	pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


	//检索绘制区域的大小。
	D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

	//通过使用 for 循环以及呈现器目标的 DrawLine 方法绘制一系列线条，以绘制网格背景。
	// Draw a grid background.
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	for (int x = 0; x < width; x += 10)
	{
		pRenderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
			m_pLightSlateGrayBrush,
			0.5f
			);
	}

	for (int y = 0; y < height; y += 10)
	{
		pRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
			m_pLightSlateGrayBrush,
			0.5f
			);
	}

	//创建两个位于屏幕中心的矩形基元。
	// Draw two rectangles.
	D2D1_RECT_F rectangle1 = D2D1::RectF(
		rtSize.width/2 - 50.0f+10*(m_time%10),
		rtSize.height/2 - 50.0f,
		rtSize.width/2 + 50.0f+10*(m_time%10),
		rtSize.height/2 + 50.0f
		);

	D2D1_RECT_F rectangle2 = D2D1::RectF(
		rtSize.width/2 - 100.0f+10*(m_time%10),
		rtSize.height/2 - 100.0f,
		rtSize.width/2 + 100.0f+10*(m_time%10),
		rtSize.height/2 + 100.0f
		);


	//使用呈现器目标的 FillRectangle 方法以及灰色画笔绘制第一个矩形的内部。
	// Draw a filled rectangle.
	pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush);

	//使用呈现器目标的 DrawRectangle 方法以及青蓝色画笔绘制第二个矩形的外部。
	// Draw the outline of a rectangle.
	pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush);

	D2D1_RECT_F rectangle3 = D2D1::RectF(
		0,
		0,
		rtSize.width,
		rtSize.height);

	std::wstringstream wss;
	wss<<L"LCNT:"<<m_fps.GetLogicFrameCount()<<L"\nLFPS:"<<m_fps.GetLogicFps()<<L"\nRCNT:"<<m_fps.GetLogicFrameCount()<<L"\nRFPS"<<m_fps.GetRenderFps();
	pRenderTarget->DrawTextW(wss.str().c_str(),wss.str().size(),m_pTextFormat,&rectangle3,m_pCornflowerBlueBrush);


	//返回 HRESULT 并关闭该方法。
	return;
}
