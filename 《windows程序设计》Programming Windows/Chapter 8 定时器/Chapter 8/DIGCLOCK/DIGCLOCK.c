#include <windows.h>
#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("DigClock");	//Dig means digital
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor= LoadCursor(hInstance, IDC_ARROW);
	wndclass.hIcon= LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hInstance= hInstance;
	wndclass.lpfnWndProc= WndProc;
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, TEXT("Digital Clcok"), WS_OVERLAPPEDWINDOW|
		WS_VSCROLL| WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void DisplayDigit(HDC hdc, int iNumber)
{
	static BOOL fSevenSegment[10][7]= {	//代表一个数字字体“日”的七条框是否需要输出
		1,1,1,0,1,1,1,	//0
		0,0,1,0,0,1,0,	//1
		1,0,1,1,1,0,1,	//2
		1,0,1,1,0,1,1,	//3
		0,1,1,1,0,1,0,	//4
		1,1,0,1,0,1,1,	//5
		1,1,0,1,1,1,1,	//6
		1,0,1,0,0,1,0,	//7
		1,1,1,1,1,1,1,	//8
		1,1,1,1,0,1,1	//9
	};

	static POINT ptSegment[7][6]={	//代表一个数字字体“日”的七条框的输出坐标。
		7,6,11,2,31,2,35,6,31,10,11,10,
		6,7,10,11,10,31,6,35,2,31,2,11,
		36,7,40,11,40,31,36,35,32,31,32,11,
		7,36,11,32,31,32,35,36,31,40,11,40,
		6,37,10,41,10,61,6,65,2,61,2,41,
		36,37,40,41,40,61,36,65,32,61,32,41,
		7,66,11,62,31,62,35,66,31,70,11,70
	};

	int iSeg;
	for(iSeg=0; iSeg<7; iSeg++)
	{
		if(fSevenSegment[iNumber][iSeg])
			Polygon(hdc, ptSegment[iSeg], 6);
	}
}

void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress)	//输出时间的数字
{
	if(!fSuppress||(iNumber/10!=0))
		DisplayDigit(hdc, iNumber/10);
	OffsetWindowOrgEx(hdc, -42, 0, NULL);
	DisplayDigit(hdc, iNumber%10);
	OffsetWindowOrgEx(hdc, -42, 0, NULL);	//平移窗口原点
}

void DisplayColon(HDC hdc)	//时间的两个冒号
{
	POINT ptColon[2][4]= {
		2,21,6,17,10,21,6,25,
		2,51,6,47,10,51,6,55
	};

	Polygon(hdc, ptColon[0], 4);
	Polygon(hdc, ptColon[1], 4);

	OffsetWindowOrgEx(hdc, -12, 0, NULL);	//平移窗口原点
}

void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress)	//分别输出当时的小时，分，秒
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	if(f24Hour)
		DisplayTwoDigits(hdc, st.wHour, FALSE);
	else
		DisplayTwoDigits(hdc, (st.wHour%=12)?st.wHour:12, fSuppress);

	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wMinute, FALSE);
	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wSecond, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL f24Hour, fSuppress;
	static HBRUSH hBrushRed;
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR szBuffer[2];
	
	switch(message)
	{
	case WM_CREATE:
		hBrushRed= CreateSolidBrush(RGB(255,0,0));
		SetTimer(hwnd, ID_TIMER, 1000, NULL);
	case WM_SETTINGCHANGE:	//如果更改了任何系统设定都会重新获得
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);	//确定是12小时制还是24小时制
		f24Hour= (szBuffer[0] == '1');

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);	//在小时制前禁止显示0
		fSuppress= (szBuffer[0] == '0');
		
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);

		return 0;
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);	//每秒使窗口无效，引发WM_PAINT
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		SetMapMode(hdc, MM_ISOTROPIC);
		SetWindowExtEx(hdc, 276, 72, NULL);
		SetViewportExtEx(hdc, cxClient, cyClient, NULL);

		SetWindowOrgEx(hdc, 138, 36, NULL);
		SetViewportOrgEx(hdc, cxClient/2, cyClient/2, NULL);
		SelectObject(hdc, GetStockObject(NULL_PEN));
		SelectObject(hdc, hBrushRed);

		DisplayTime(hdc, f24Hour, fSuppress);
		
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER);
		DeleteObject(hBrushRed);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}