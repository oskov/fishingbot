#include "stdafx.h"
#include "Bot.h"
#define MS_PER_FRAME 100
Bot::Bot(HWND hwnd) :hwnd(hwnd)
{
	init();

	
}


Bot::~Bot()
{
	// отпускаем ресурсы
	d3d->Release();
	surface->Release();
	device->Release();
}

void Bot::start()
{	
	cycle();
}

void Bot::stopRun()
{
	run = false;
}

void Bot::setDebugHwnd(HWND hwnd)
{
	debug = hwnd;
}

void Bot::startFishing()
{
	Sleep(800);
	missCounter = 0;
	fishing = true;
	PressButton(10);
	objects.clear();
	startTime = system_clock::now();
	Sleep(500);
}

void Bot::processObjects(Rect obj)
{
	if (objects.size() == 100) {
		objects.erase(objects.begin());
	}
	objects.push_back(obj);
	if (objects.size() > 10) {
		double averageArea = objects[0].area();
		double averagep = objects[0].width + objects[0].height;
		for (int i = 1; i < objects.size(); i++) {
			averagep = (averagep+objects[i].width+objects[i].height)/2;
			averageArea = (averageArea + objects[i].area()) / 2;


		}
		if (averageArea - obj.area() > 160) {//100
			std::string s = std::to_string(averageArea - obj.area());
			std::wstring stemp = std::wstring(s.begin(), s.end());
			LPCWSTR sw = stemp.c_str();
			OutputDebugString(sw);
			OutputDebugString(_TEXT("\n"));
				fishing = false;
				moveMouseLine(obj.x+obj.width/2,obj.y+obj.height/2);
				Sleep(50);
 				leftClick(100);
				Sleep(100);
				moveMouseLine(rand() % 1000 + 200, rand() % 500 + 150);
				fishGot++;
		}
		else {
			if (std::chrono::duration_cast<std::chrono::seconds>(startTime - system_clock::now()).count() > 20) {
				fishing = false;
				fishLost++;
			}

		}
	}

}

void Bot::init()
{
// init D3D and get screen size
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = mode.Height;
	parameters.BackBufferWidth = mode.Width;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;

	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device);
	device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);
	surface->LockRect(&rc, NULL, 0);
	pitch = rc.Pitch;
	surface->UnlockRect();
	shot = new BYTE[pitch * mode.Height];
}

void Bot::cycle()
{
	auto fishingStart = std::chrono::system_clock::now();
	auto startTime = std::chrono::system_clock::now();
	auto currentTime = startTime;
	int i = 0;
	run = true;
	while (run) {
		if (!fishing)startFishing();
			startTime = std::chrono::system_clock::now();
			process();
			currentTime = std::chrono::system_clock::now();
			long timeWait = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
			if (GetAsyncKeyState(VK_RETURN) == -32767)run = false;;
			if (timeWait < MS_PER_FRAME)Sleep(MS_PER_FRAME - timeWait);
	}
	auto fishingEnd = std::chrono::system_clock::now();
	long timeWait = std::chrono::duration_cast<std::chrono::seconds>(fishingEnd - fishingStart).count();






}
int i = 0;
void Bot::process()
{
	if(debug!=NULL)
		SetWindowPos(debug, HWND_TOPMOST,10000,0,1920,1080, SWP_NOOWNERZORDER);
	Direct3D9TakeScreenshots(D3DADAPTER_DEFAULT, img);
	if (debug != NULL)
			SetWindowPos(debug, HWND_TOP,10000 , 0, 1920, 1080, SWP_NOACTIVATE);
	auto mat = ImgToMat(img);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	GaussianBlur(mat, mat, Size(5, 5), 0);
	cvtColor(mat, mat, COLOR_BGR2GRAY);
	Canny(mat, mat, 150, 200);
	morphologyEx(mat, mat, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(3, 3)));
	findContours(mat, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	if (contours.size() != 0) {
		vector<Point> largest;
		double maxContour = -1;
		for (vector<Point> m : contours) {
			if (contourArea(m) > maxContour) {
				largest = m;
				maxContour = contourArea(largest);
			}

		}
		
		Rect object = boundingRect(largest);
		//mat = Scalar(0, 0, 0);
		rectangle(mat, object.br(), object.tl(),Scalar(250, 50, 50), 1);
		processObjects(object);
		//moveMouseLine(object.x+object.width/2,object.y+object.height/2);
		//bitmap = ConvertCVMatToBMP(mat);
		
		ShowWindow(debug, SW_MAXIMIZE);
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		//if(++i%50)
		//cv::imwrite(to_string(chrono::system_clock::now().time_since_epoch().count())+".png",mat);
		//PostMessage(hwnd, WM_USER + 1, 0, 0);
		missCounter = 0;
	}
	else {
		missCounter++;
		if (missCounter == 10) {
		fishing = false;
		fishLost++;
		}
	}
}

void Bot::moveMouse(int x, int y)
{

		double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
		double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
		double fx = x * (65535.0f / fScreenWidth);
		double fy = y * (65535.0f / fScreenHeight);
		INPUT  Input = { 0 };
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		Input.mi.dx = fx;
		Input.mi.dy = fy;
		SendInput(1, &Input, sizeof(INPUT));
	
}

void Bot::leftClick(int pause)
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));
	Sleep(pause);
	// left up
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

	//TODO REMOVe
	Input = { 0 };
	// left down 
	Input.type = INPUT_KEYBOARD;
	Input.ki.wScan = 0;
	Input.ki.time = 0;
	Input.ki.dwExtraInfo = 0;

	// Press the "A" key
	Input.ki.wVk = 0x41; // virtual-key code for the "a" key
	Input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(pause*25);
	// Release the "A" key
	Input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &Input, sizeof(INPUT));
}

void Bot::rightClick(int pause)
{
	INPUT    Input = { 0 };
	// right down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(pause);
	// right up
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &Input, sizeof(INPUT));
}
void Bot::PressButton(int pause)
{
	INPUT    Input = { 0 };
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = VK_OEM_5; // / code
	Input.ki.wScan = 0;
	Input.ki.dwFlags = 0; // key down
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(pause);
	Input.ki.dwFlags = KEYEVENTF_KEYUP; // key up
	SendInput(1, &Input, sizeof(INPUT));
}

void Bot::moveMouseLine(int endX, int endY)
{
	int d = 0;
	POINT p;
	GetCursorPos(&p);
	
	int currentX = p.x;
	int currentY = p.y;
	int dx = abs(endX - currentX);
	int dy = abs(endY - currentY);

	int dx2 = 2 * dx; // slope scaling factors to
	int dy2 = 2 * dy; // avoid floating point

	int ix = currentX < endX ? 1 * Const::MIN_MOUSE_SPEED : -1 * Const::MIN_MOUSE_SPEED; // increment direction
	int iy = currentY < endY ? 1 * Const::MIN_MOUSE_SPEED : -1 * Const::MIN_MOUSE_SPEED;

	int x = currentX;
	int y = currentY;
	int pauseLenght = 500/(dx+dy+1)+10;
	if (dx >= dy) {
		while (true) {
			moveMouse(x, y);
			if (abs(x - endX) < Const::MAX_MOUSE_SPEED) {
				moveMouse(endX, endY);

				break;
			}
			x += ix;
			d += dy2;
			if (d > dx) {
				y += iy;
				d -= dx2;
			}
			Sleep(pauseLenght);
		}
	}
	else {
		while (true) {
			moveMouse(x, y);
			if (abs(y - endY) < Const::MAX_MOUSE_SPEED) {
				moveMouse(endX, endY);
				break;
			}
			y += iy;
			d += dx2;
			if (d > dy) {
				x += ix;
				d -= dy2;
			}
			Sleep(pauseLenght);
		}
	}
}

cv::Rect Bot::getObjectRect()
{
	cv::Rect rect(0,0,img.width,img.height);



	return cv::Rect();
}
	

void Bot::Direct3D9TakeScreenshots(UINT adapter, Image & img)
{
	// get the data
	device->GetFrontBufferData(0, surface);
	// copy it into our buffers
	surface->LockRect(&rc, NULL, 0);
	CopyMemory(shot, rc.pBits, rc.Pitch * mode.Height);
	surface->UnlockRect();
	img.bytes = shot;
	img.pitch = pitch;
	img.height = mode.Height;
	img.width = mode.Width;

}

cv::Mat Bot::ImgToMat(Image img)
{
	return cv::Mat(img.height, img.width, CV_8UC4, (BYTE*)img.bytes);
}
// MEMORY LEAK
auto Bot::ConvertCVMatToBMP(cv::Mat frame) -> HBITMAP

{
	auto convertOpenCVBitDepthToBits = [](const int32_t value)
	{
		auto regular = 0u;

		switch (value)
		{
		case CV_8U:
		case CV_8S:
			regular = 8u;
			break;

		case CV_16U:
		case CV_16S:
			regular = 16u;
			break;

		case CV_32S:
		case CV_32F:
			regular = 32u;
			break;

		case CV_64F:
			regular = 64u;
			break;

		default:
			regular = 0u;
			break;
		}

		return regular;
	};

	auto imageSize = frame.size();
	assert(imageSize.width && "invalid size provided by frame");
	assert(imageSize.height && "invalid size provided by frame");

	if (imageSize.width && imageSize.height)
	{
		auto headerInfo = BITMAPINFOHEADER{};
		ZeroMemory(&headerInfo, sizeof(headerInfo));

		headerInfo.biSize = sizeof(headerInfo);
		headerInfo.biWidth = imageSize.width;
		headerInfo.biHeight = -(imageSize.height); // negative otherwise it will be upsidedown
		headerInfo.biPlanes = 1;// must be set to 1 as per documentation frame.channels();

		const auto bits = convertOpenCVBitDepthToBits(frame.depth());
		headerInfo.biBitCount = frame.channels() * bits;

		auto bitmapInfo = BITMAPINFO{};
		ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));

		bitmapInfo.bmiHeader = headerInfo;
		bitmapInfo.bmiColors->rgbBlue = 0;
		bitmapInfo.bmiColors->rgbGreen = 0;
		bitmapInfo.bmiColors->rgbRed = 0;
		bitmapInfo.bmiColors->rgbReserved = 0;

		auto dc = GetDC(nullptr);
		assert(dc != nullptr && "Failure to get DC");
		auto bmp = CreateDIBitmap(dc,
			&headerInfo,
			CBM_INIT,
			frame.data,
			&bitmapInfo,
			DIB_RGB_COLORS);
		assert(bmp != nullptr && "Failure creating bitmap from captured frame");

		return bmp;
	}
	else
	{
		return nullptr;
	}
}
