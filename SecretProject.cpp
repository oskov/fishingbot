// SecretProject.cpp : Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "SecretProject.h"
#include <atlstr.h> 
#include<thread>
#include"Bot.h"

#define MAX_LOADSTRING 100


HHOOK hHookKB2 = NULL;
HHOOK hook2 = NULL;
MSG messages;
using namespace std;
int RemoveHook(HHOOK hook)
{
	BOOL Hbool = 0;
	int err = 0;
	Hbool = UnhookWindowsHookEx(hook);
	if (Hbool == 0)
	{
		err = (int)GetLastError();
		return err;
	}
	return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= HC_ACTION)
	{
		KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
		if (wParam == WM_KEYUP || wParam == WM_KEYDOWN) {
			if (wParam == WM_KEYUP) {
				p->flags = 128;
			}
			else {
				p->flags = 0;
			}

			return CallNextHookEx(hook2, nCode, wParam, lParam);
		}
	}
	return CallNextHookEx(hook2, nCode, wParam, lParam);
}


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= HC_ACTION)
	{

		MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
		if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP || wParam == WM_MOUSEMOVE) {
			p->flags = 0;
			return CallNextHookEx(hHookKB2, nCode, wParam, lParam);;
		}



	}
	return CallNextHookEx(hHookKB2, nCode, wParam, lParam);
}

int SetHook()
{
	int err = 0;
	hHookKB2 = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseProc, GetModuleHandle(NULL), 0);
	if (hHookKB2 == NULL)
	{
		err = (int)GetLastError();
		return err;
	}
	return 0;
}
int SetHook2()
{
	int err = 0;
	hook2 = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	if (hook2 == NULL)
	{
		err = (int)GetLastError();
		return err;
	}
	return 0;
}

/*
Image Direct3D9TakeScreenshots(UINT adapter)
{
	IDirect3D9 *d3d = nullptr;
	IDirect3DDevice9 *device = nullptr;
	IDirect3DSurface9 *surface = nullptr;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	D3DDISPLAYMODE mode;
	D3DLOCKED_RECT rc;
	UINT pitch;
	LPBYTE shot = nullptr;
	Image img;
	
	// init D3D and get screen size
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	d3d->GetAdapterDisplayMode(adapter, &mode);

	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = mode.Height;
	parameters.BackBufferWidth = mode.Width;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;
	
	// create device & capture surface
	d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device);
	device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);

	// compute the required buffer size
	surface->LockRect(&rc, NULL, 0);
	pitch = rc.Pitch;
	surface->UnlockRect();
	// get the data
	device->GetFrontBufferData(0, surface);
	// copy it into our buffers
	surface->LockRect(&rc, NULL, 0);
	shot = new BYTE[pitch * mode.Height];
	if (shot == nullptr)exit(1);
	CopyMemory(shot, rc.pBits, rc.Pitch * mode.Height);
	surface->UnlockRect();
	img.bytes = shot;
	img.pitch = pitch;
	img.height = mode.Height;
	img.width = mode.Width;
	// отпускаем ресурсы
	d3d->Release();
	surface->Release();
	device->Release();
	return img;
}
*/

/*
void test() {
	Image img = Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT);
	cv::Mat D3DSurface(img.height, img.width, CV_8UC4, (BYTE*)img.bytes);
	//cv::imwrite("WRTF.jpg", D3DSurface);
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
	
	for (int i = 0; i < 100; i++) {
		Image img = Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT);
		cv::Mat D3DSurface(img.height, img.width, CV_8UC4, (BYTE*)img.bytes);
	imshow("Display window", D3DSurface);
	Sleep(100);
	}
}*/
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWindow;									// Main window
Bot bot(FindWindow(NULL,L"World of Warcraft"));
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

	//Bot bot(D3DADAPTER_DEFAULT);
	SetHook();
	SetHook2();
	std::thread test(&Bot::start,&bot);
	//bot.start();\\
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SECRETPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	MSG msg = {};
	
    // Цикл основного сообщения:
	while (WM_QUIT != msg.message)
	{
		if (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (GetAsyncKeyState(VK_DOWN) == -32767)ShowWindow(hWindow, SW_HIDE);
		if (GetAsyncKeyState(VK_UP) == -32767)ShowWindow(hWindow, SW_SHOW);
	}
	bot.stopRun();
	test.join();
	RemoveHook(hHookKB2);
	RemoveHook(hook2);
    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SECRETPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SECRETPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hWindow = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWindow)
   {
      return FALSE;
   }

   ShowWindow(hWindow, nCmdShow);
   UpdateWindow(hWindow);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

	case WM_CREATE:
	{	/*
		HWND button1 = CreateWindow(TEXT("button"), TEXT("Button"),
			WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
			80, 10, 100, 50,
			hWnd, (HMENU)1, NULL, NULL);
		
		*/
		bot.setDebugHwnd(hWnd);
		break;
	}
	case WM_SIZE:
		{

		break;
		}
		
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			BITMAP          bitmap;
			HDC             hdcMem;
			HGDIOBJ         oldBitmap;
			hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, bot.bitmap);

			GetObject(bot.bitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);

		/*	Image img = Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT);
			cv::Mat D3DSurface(img.height, img.width, CV_8UC4, (BYTE*)img.bytes);
			cv::cvtColor(D3DSurface,D3DSurface, CV_BGR2GRAY);
			std::string s = std::to_string(D3DSurface.type());
			std::wstring stemp = std::wstring(s.begin(), s.end());
			LPCWSTR sw = stemp.c_str();
			OutputDebugString(sw);
			HBITMAP hBitmap = ConvertCVMatToBMP(D3DSurface);


			BITMAP bitmap;
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			HDC hdcMem = CreateCompatibleDC(hdc);
			HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);

			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);*/
			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
