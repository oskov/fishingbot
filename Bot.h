#pragma once
#include <chrono>
#include <cmath>
#include "stdafx.h"
#include "Consts.h"
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;
using namespace chrono;
class Bot
{

public:
	Bot(HWND hwnd);
	~Bot();
	void start();
	void stopRun();
	void setDebugHwnd(HWND hwnd);
	HBITMAP bitmap;
private :
	struct Image {
		LPBYTE bytes;
		UINT height;
		UINT width;
		UINT pitch;
	}img;
	HWND hwnd;
	HWND debug;

	vector<Rect> objects;

	bool run = true;
	int endTime;
	system_clock::time_point  startTime;
	bool fishing = false;
	int missCounter;
	int fishGot;
	int fishLost;


	IDirect3D9 *d3d = nullptr;
	IDirect3DDevice9 *device = nullptr;
	IDirect3DSurface9 *surface = nullptr;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	D3DDISPLAYMODE mode;
	D3DLOCKED_RECT rc;
	UINT pitch;
	LPBYTE shot = nullptr;
	 
	
	void startFishing();
	void processObjects(Rect obj);
	void init();
	void cycle();
	void process();
	void moveMouse(int x, int y);
	void leftClick(int pause);
	void rightClick(int pause);
	void PressButton(int pause);
	void moveMouseLine(int endX, int endY);
	cv::Rect getObjectRect();
	void Direct3D9TakeScreenshots(UINT adapter, Image &img);
	cv::Mat ImgToMat(Image img);
	auto ConvertCVMatToBMP(cv::Mat frame)->HBITMAP;
	

};

