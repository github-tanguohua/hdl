// hdl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "hdl.h"
#include "Game.h"






#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//声明用于初始化类、创建和丢弃资源、处理消息循环、呈现内容和窗口过程的方法。
class DemoApp
{
public:
	DemoApp();
	~DemoApp();

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Process and dispatch messages
	void RunMessageLoop();

private:
	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources();

	void OnRender();

	// Resize the render target.
	void OnResize(
		UINT width,
		UINT height
		);

	void OnUpdate();

	// The windows procedure.
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	Game m_game;
};

//以类成员的形式为一个 ID2D1Factory 对象、一个 ID2D1HwndRenderTarget 对象和两个 ID2D1SolidColorBrush 对象声明指针。


//第 2 部分：实现类基础结构
//在本部分中，您将实现 DemoApp 构造函数和析构函数、其初始化和消息循环方法以及 WinMain 函数。其中大多数方法看起来都与其他任何 Win32 应用程序中的方法相同。唯一的例外在于 Initialize 方法，该方法调用可创建多个 Direct2D 资源的 CreateDeviceIndependentResources 方法（在下一部分中定义该方法）。
//在类实现文件中，实现类构造函数和析构函数。构造函数应将其成员初始化为 NULL。析构函数应释放作为类成员存储的所有接口。
DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL)
{
}
DemoApp::~DemoApp()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);

}

//实现用于转换和调度消息的 DemoApp::RunMessageLoop 方法。
void DemoApp::RunMessageLoop()
{
	DWORD const logicfps=25;
	DWORD timedelt=1000/logicfps;
	DWORD last=GetTickCount();
	MSG msg;

	bool gameover = false;  
	while(!gameover)  
	{  
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))  
		{  
			//hand any event message  
			TranslateMessage(&msg);  
			DispatchMessage(&msg);  
		}
		DWORD now=GetTickCount();
		if(now-last>=timedelt)
		{
			last=now;
			OnUpdate();
			now=GetTickCount();
			if(now-last<timedelt)
			{
				OnRender();
			}
		}
	}
}

//实现用于创建窗口、显示该窗口并调用 DemoApp::CreateDeviceIndependentResources 方法的 Initialize 方法。您将在下一节中实现 CreateDeviceIndependentResources 方法。
HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = DemoApp::WndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = sizeof(LONG_PTR);
		wcex.hInstance     = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName  = NULL;
		wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DDemoApp";

		RegisterClassEx(&wcex);


		// Because the CreateWindow function takes its size in pixels,
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;

		// The factory returns the current system DPI. This is also the value it will use
		// to create its own windows.
		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


		// Create the window.
		m_hwnd = CreateWindow(
			L"D2DDemoApp",
			L"Direct2D Demo App",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}


//第 3 部分：创建 Direct2D 资源
//在本部分中，您将创建用于绘制的 Direct2D 资源。Direct2D 提供两种类型的资源：与设备无关的资源（可在应用程序持续期间保持）和与设备有关的资源。与设备有关的资源与特定呈现设备关联，在移除该设备后将停止发挥作用。
//实现 DemoApp::CreateDeviceIndependentResources 方法。在该方法中，创建一个与设备无关的资源 ID2D1Factory，用于创建其他 Direct2D 资源。使用 m_pDirect2DdFactory 类成员存储工厂。
HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

//第 4 部分：呈现 Direct2D 内容
//在本部分中，您将实现窗口过程、用于绘制内容的 OnRender 方法以及用于在调整窗口大小时调整呈现器目标大小的 OnResize 方法。
//实现用于处理窗口消息的 DemoApp::WndProc 方法。对于 WM_SIZE 消息，将调用 DemoApp::OnResize 方法并向该方法传递新的宽度和高度。对于 WM_PAINT 和 WM_DISPLAYCHANGE 消息，将调用 DemoApp::OnRender 方法来绘制窗口。您将在接下来的步骤中实现 OnRender 和 OnResize 方法。
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pDemoApp)
			);

		result = 1;
	}
	else
	{
		DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pDemoApp->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;

			case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

			case WM_PAINT:
				{
					pDemoApp->OnRender();
					ValidateRect(hwnd, NULL);
				}
				result = 0;
				wasHandled = true;
				break;

			case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


void DemoApp::OnRender()
{
	HRESULT hr = S_OK;

	if(!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// Create a Direct2D render target.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);
	
		if (SUCCEEDED(hr))
		{
			hr = m_game.CreateDeviceResources(m_pDirect2dFactory,m_pRenderTarget);
		}
	}

	//验证 CreateDeviceResource 方法是否已成功。如果该方法未成功，则不执行任何绘制。
	if (SUCCEEDED(hr))
	{
		//在您刚创建的 if 语句中，通过调用呈现器目标的 BeginDraw 方法启动绘制。将呈现器目标的转换设置为单位矩阵，并清除窗口。
		m_pRenderTarget->BeginDraw();

		m_game.OnRender(m_pRenderTarget);

		//调用呈现器目标的 EndDraw 方法。EndDraw 方法会返回 HRESULT，以指示绘制操作是否成功。关闭在步骤 3 中开始的 if 语句。
		hr = m_pRenderTarget->EndDraw();
	}

	//检查 EndDraw 返回的 HRESULT。如果 HRESULT 指示需要重新创建呈现器目标，请调用 DemoApp::DiscardDeviceResources 方法释放该呈现器目标；在窗口下次接收 WM_PAINT 或 WM_DISPLAYCHANGE 消息时会重新创建呈现器目标。
	if (hr == D2DERR_RECREATE_TARGET)
	{
		SafeRelease(&m_pRenderTarget);
		m_game.DiscardDeviceResources();
	}

	//返回 HRESULT 并关闭该方法。
	return;
}


//实现 DemoApp::OnResize 方法，使其将呈现器目标的大小调整为窗口的新尺寸。
void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

void DemoApp::OnUpdate()
{
	m_game.OnUpdate();
}

//创建用作应用程序入口点的 WinMain 方法。初始化 DemoApp 类的实例并开始其消息循环。
int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	// Use HeapSetInformation to specify that the process should
	// terminate if the heap manager detects an error in any heap used
	// by the process.
	// The return value is ignored, because we want to continue running in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}
