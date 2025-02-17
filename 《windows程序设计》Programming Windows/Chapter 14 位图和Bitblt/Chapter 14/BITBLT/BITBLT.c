/*
初步使用BitBlt函数。
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("BitBlt");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_INFORMATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("BitBlt Demo"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient, cxSource, cySource;
	HDC hdcClient, hdcWindow;
	int x,y;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		//SM_CXSIZEFRAME 围绕可改变大小的窗口的边框的厚度 
		//SM_CXSIZE 以像素计算的标题栏按钮的尺寸
		//SM_CYCAPTION 以像素计算的普通窗口标题的高度
		cxSource= GetSystemMetrics(SM_CXSIZEFRAME)+ GetSystemMetrics(SM_CXSIZE);	
		cySource= GetSystemMetrics(SM_CYSIZEFRAME)+ GetSystemMetrics(SM_CYCAPTION);
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdcClient= BeginPaint(hwnd, &ps);
		hdcWindow= GetWindowDC(hwnd);

		for(y=0; y<cyClient; y+= cySource)
			for(x=0; x<cxClient; x+= cxSource)
			{
				BitBlt(hdcClient, x,y, cxSource, cySource, hdcWindow, 0,0, SRCCOPY);
			}
			
		ReleaseDC(hwnd, hdcWindow);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}