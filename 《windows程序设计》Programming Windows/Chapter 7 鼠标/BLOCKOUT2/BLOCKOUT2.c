#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("BlokOut2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Mouse Button &Capture Demo"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void DrawBoxOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)//根据平移鼠标画空的方框
{
	HDC hdc;
	hdc= GetDC(hwnd);
	SetROP2(hdc, R2_NOT);//设定前景色为后景色的反色，即白色变为黑色作为矩形的颜色
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
	ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL fBlocking, fValidBox;	//fBlocking代表鼠标down的时候，fValidBox代表鼠标up的时候
	static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
	HDC hdc;
	PAINTSTRUCT ps;
	switch(message)
	{
	case WM_LBUTTONDOWN:
		ptBeg.x= ptEnd.x= LOWORD(lParam);
		ptBeg.y= ptEnd.y= HIWORD(lParam);

		DrawBoxOutline(hwnd, ptBeg, ptEnd);

		SetCapture(hwnd);//拦截鼠标消息
		SetCursor(LoadCursor(NULL, IDC_CROSS));	//十字鼠标箭头

		fBlocking= TRUE;//按下状态该bool为true
		return 0;

	case WM_MOUSEMOVE:
		if(fBlocking)
		{
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ptEnd.x= LOWORD(lParam);
			ptEnd.y= HIWORD(lParam);

			DrawBoxOutline(hwnd, ptBeg, ptEnd);
		}
		return 0;
	case WM_LBUTTONUP:
		if(fBlocking)
		{
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ptBoxBeg= ptBeg;
			ptBoxEnd.x= LOWORD(lParam);
			ptBoxEnd.y= HIWORD(lParam);

			ReleaseCapture();//释放拦截鼠标消息
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			fBlocking= FALSE;
			fValidBox= TRUE;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_CHAR:
		if(fBlocking&wParam == '\x1B')
		{
			DrawBoxOutline(hwnd, ptBeg, ptEnd);
			ReleaseCapture();//释放拦截鼠标消息
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			fBlocking= FALSE;
		}
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		if(fValidBox)
		{
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
		}
		if(fBlocking)
		{
			SetROP2(hdc, R2_NOT);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//仅添加了拦截鼠标函数SetCapture(hwnd)以及释放拦截鼠标函数