/*
直接用代码来把单色位图给写出来，恕我现在心中算力不足。
与BRICKS1的区别是，将位信息传入bitmap然后使用bitmap来创建位图
*/
#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("Bricks2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("CreateBitmap Demo"), WS_OVERLAPPEDWINDOW, 
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
	static BITMAP bitmap= {0,8,8,2,1,1};//8长8宽
	static BYTE bits[8][2]=
	{
		//这些十六进制数字，一个数字能转化成8位二进制数字，每位0或1代表黑色或白色的一个色块
		0xFF, 0, 0x0C, 0, 0x0C, 0, 0x0C, 0, 
		0xFF, 0, 0xC0, 0, 0xC0, 0, 0xC0, 0
	};
	static HBITMAP hBitmap;
	static int cxClient, cyClient, cxSource, cySource;
	HDC hdc, hdcMem;
	int x,y;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		bitmap.bmBits= bits;	//将位信息传入bitmap
		hBitmap= CreateBitmapIndirect(&bitmap);	//使用bitmap来创建位图
		cxSource= bitmap.bmWidth;
		cySource= bitmap.bmHeight;
		return 0;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);

		hdcMem= CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);

		for(y=0; y<cyClient; y+= cySource)
			for(x=0; x<cxClient; x+= cxSource)
			{
				BitBlt(hdc, x, y, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
			}
		
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}