// hdl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "hdl.h"
#include "Game.h"






#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//�������ڳ�ʼ���ࡢ�����Ͷ�����Դ��������Ϣѭ�����������ݺʹ��ڹ��̵ķ�����
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

//�����Ա����ʽΪһ�� ID2D1Factory ����һ�� ID2D1HwndRenderTarget ��������� ID2D1SolidColorBrush ��������ָ�롣


//�� 2 ���֣�ʵ��������ṹ
//�ڱ������У�����ʵ�� DemoApp ���캯�����������������ʼ������Ϣѭ�������Լ� WinMain ���������д�����������������������κ� Win32 Ӧ�ó����еķ�����ͬ��Ψһ���������� Initialize �������÷������ÿɴ������ Direct2D ��Դ�� CreateDeviceIndependentResources ����������һ�����ж���÷�������
//����ʵ���ļ��У�ʵ���๹�캯�����������������캯��Ӧ�����Ա��ʼ��Ϊ NULL����������Ӧ�ͷ���Ϊ���Ա�洢�����нӿڡ�
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

//ʵ������ת���͵�����Ϣ�� DemoApp::RunMessageLoop ������
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

//ʵ�����ڴ������ڡ���ʾ�ô��ڲ����� DemoApp::CreateDeviceIndependentResources ������ Initialize ��������������һ����ʵ�� CreateDeviceIndependentResources ������
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


//�� 3 ���֣����� Direct2D ��Դ
//�ڱ������У������������ڻ��Ƶ� Direct2D ��Դ��Direct2D �ṩ�������͵���Դ�����豸�޹ص���Դ������Ӧ�ó�������ڼ䱣�֣������豸�йص���Դ�����豸�йص���Դ���ض������豸���������Ƴ����豸��ֹͣ�������á�
//ʵ�� DemoApp::CreateDeviceIndependentResources �������ڸ÷����У�����һ�����豸�޹ص���Դ ID2D1Factory�����ڴ������� Direct2D ��Դ��ʹ�� m_pDirect2DdFactory ���Ա�洢������
HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

//�� 4 ���֣����� Direct2D ����
//�ڱ������У�����ʵ�ִ��ڹ��̡����ڻ������ݵ� OnRender �����Լ������ڵ������ڴ�Сʱ����������Ŀ���С�� OnResize ������
//ʵ�����ڴ�������Ϣ�� DemoApp::WndProc ���������� WM_SIZE ��Ϣ�������� DemoApp::OnResize ��������÷��������µĿ�Ⱥ͸߶ȡ����� WM_PAINT �� WM_DISPLAYCHANGE ��Ϣ�������� DemoApp::OnRender ���������ƴ��ڡ������ڽ������Ĳ�����ʵ�� OnRender �� OnResize ������
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

	//��֤ CreateDeviceResource �����Ƿ��ѳɹ�������÷���δ�ɹ�����ִ���κλ��ơ�
	if (SUCCEEDED(hr))
	{
		//�����մ����� if ����У�ͨ�����ó�����Ŀ��� BeginDraw �����������ơ���������Ŀ���ת������Ϊ��λ���󣬲�������ڡ�
		m_pRenderTarget->BeginDraw();

		m_game.OnRender(m_pRenderTarget);

		//���ó�����Ŀ��� EndDraw ������EndDraw �����᷵�� HRESULT����ָʾ���Ʋ����Ƿ�ɹ����ر��ڲ��� 3 �п�ʼ�� if ��䡣
		hr = m_pRenderTarget->EndDraw();
	}

	//��� EndDraw ���ص� HRESULT����� HRESULT ָʾ��Ҫ���´���������Ŀ�꣬����� DemoApp::DiscardDeviceResources �����ͷŸó�����Ŀ�ꣻ�ڴ����´ν��� WM_PAINT �� WM_DISPLAYCHANGE ��Ϣʱ�����´���������Ŀ�ꡣ
	if (hr == D2DERR_RECREATE_TARGET)
	{
		SafeRelease(&m_pRenderTarget);
		m_game.DiscardDeviceResources();
	}

	//���� HRESULT ���رո÷�����
	return;
}


//ʵ�� DemoApp::OnResize ������ʹ�佫������Ŀ��Ĵ�С����Ϊ���ڵ��³ߴ硣
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

//��������Ӧ�ó�����ڵ�� WinMain ��������ʼ�� DemoApp ���ʵ������ʼ����Ϣѭ����
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
