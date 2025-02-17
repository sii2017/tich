#include <windows.h>
#define ID_SMALLER 1
#define ID_LARGER 2
#define BTN_WIDTH (8*cxChar)
#define BTN_HEIGHT (4*cyChar)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("OwnDraw");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;	//首次加入
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG"), szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd= CreateWindow(szAppName, TEXT("Owner-Draw Button Demo"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

void Triangle(HDC hdc, POINT pt[])
{
	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	Polygon(hdc, pt, 3);
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndSmaller, hwndLarger;
	static int cxClient, cyClient, cxChar, cyChar;
	int cx, cy;
	LPDRAWITEMSTRUCT pdis;
	POINT pt[3];
	RECT rc;
	switch(message)
	{
	case WM_CREATE:
		cxChar= LOWORD(GetDialogBaseUnits());
		cyChar= HIWORD(GetDialogBaseUnits());

		hwndSmaller= CreateWindow(TEXT("button"), TEXT(""), 
			WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,				//BS_OWNERDRAW这种类型下，在该按钮的范围里按下都是点击按钮
			0,0, BTN_WIDTH, BTN_HEIGHT,  
			hwnd, (HMENU)ID_SMALLER, hInst, NULL);
		
		hwndLarger= CreateWindow(TEXT("button"), TEXT(""), 
			WS_CHILD|WS_VISIBLE|BS_OWNERDRAW, 
			0,0, BTN_WIDTH, BTN_HEIGHT,  
			hwnd, (HMENU)ID_LARGER, hInst, NULL);
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);

		MoveWindow(hwndSmaller, cxClient/2-3*BTN_WIDTH/2, cyClient/2- BTN_HEIGHT/2, 
			BTN_WIDTH, BTN_HEIGHT, TRUE);
		MoveWindow(hwndLarger, cxClient/2 + BTN_WIDTH/2, cyClient/2 - BTN_HEIGHT/2, 
			BTN_WIDTH, BTN_HEIGHT, TRUE);
		return 0;
	case WM_COMMAND:
		GetWindowRect(hwnd, &rc);	//获得当前窗口的大小（上下左右的坐标）以便进行缩小放大
		//Make the winodw 10% smaller or larger
		switch(wParam)
		{
		case ID_SMALLER:
			rc.left += cxClient/20;
			rc.right -= cxClient/20;
			rc.top += cyClient/20;
			rc.bottom -= cyClient/20;
			break;
		case ID_LARGER:
			rc.left -= cxClient/20;
			rc.right += cxClient/20;
			rc.top -= cyClient/20;
			rc.bottom += cyClient/20;
			break;
		}
		MoveWindow(hwnd, rc.left, rc.top, rc.right-rc.left, rc.bottom- rc.top, TRUE);
		return 0;
	case WM_DRAWITEM://当BS_OWNERDRAW的按钮被按下放开或者需要重新着色的时候都会发送该消息
		pdis= (LPDRAWITEMSTRUCT)lParam;	//存储了子窗口控件的大量信息报货hdc等等
		//fill area with white and frame it black  白底黑框
		FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
		//Draw inward and outward balck triangles
		cx= pdis->rcItem.right - pdis->rcItem.left;
		cy= pdis->rcItem.bottom - pdis->rcItem.top;
		
		switch(pdis->CtlID)	//用来重画子控件按钮的方框
		{
		case ID_SMALLER:
			pt[0].x= 3*cx/8;
			pt[0].y= 1*cy/8;
			pt[1].x= 5*cx/8;
			pt[1].y= 1*cy/8;
			pt[2].x= 4*cx/8;
			pt[2].y= 3*cy/8;

			Triangle(pdis->hDC,pt);		//画按钮上的三角形的

			pt[0].x= 7*cx/8;
			pt[0].y= 3*cy/8;
			pt[1].x= 7*cx/8;
			pt[1].y= 5*cy/8;
			pt[2].x= 5*cx/8;
			pt[2].y= 4*cy/8;

			Triangle(pdis->hDC, pt);

			pt[0].x= 5*cx/8;
			pt[0].y= 7*cy/8;
			pt[1].x= 3*cx/8;
			pt[1].y= 7*cy/8;
			pt[2].x= 4*cx/8;
			pt[2].y= 5*cy/8;

			Triangle(pdis->hDC, pt);

			pt[0].x= 1*cx/8;
			pt[0].y= 5*cy/8;
			pt[1].x= 1*cx/8;
			pt[1].y= 3*cy/8;
			pt[2].x= 3*cx/8;
			pt[2].y= 4*cy/8;

			Triangle(pdis->hDC, pt);
			break;
		case ID_LARGER:
			pt[0].x= 5*cx/8;
			pt[0].y= 3*cy/8;
			pt[1].x= 3*cx/8;
			pt[1].y= 3*cy/8;
			pt[2].x= 4*cx/8;
			pt[2].y= 1*cy/8;

			Triangle(pdis->hDC, pt);

			pt[0].x= 5*cx/8;
			pt[0].y= 5*cy/8;
			pt[1].x= 5*cx/8;
			pt[1].y= 3*cy/8;
			pt[2].x= 7*cx/8;
			pt[2].y= 4*cy/8;

			Triangle(pdis->hDC, pt);

			pt[0].x= 3*cx/8;
			pt[0].y= 5*cy/8;
			pt[1].x= 5*cx/8;
			pt[1].y= 5*cy/8;
			pt[2].x= 4*cx/8;
			pt[2].y= 7*cy/8;

			Triangle(pdis->hDC, pt);

			pt[0].x= 3*cx/8;
			pt[0].y= 3*cy/8;
			pt[1].x= 3*cx/8;
			pt[1].y= 5*cy/8;
			pt[2].x= 1*cx/8;
			pt[2].y= 4*cy/8;

			Triangle(pdis->hDC, pt);
			break;
		}
		//Invert the rectangle if the button is selected
		if(pdis->itemState&ODS_SELECTED)	//如果正在按下按钮
			InvertRect(pdis->hDC, &pdis->rcItem);	//将按钮设为反转的颜色
		//Draw a focus rectangle if the button hase the focus
		if(pdis->itemState&ODS_FOCUS)
		{
			pdis->rcItem.left+= cx/16;
			pdis->rcItem.top+= cy/16;
			pdis->rcItem.right-= cx/16;
			pdis->rcItem.bottom-= cy/16;

			DrawFocusRect(pdis->hDC, &pdis->rcItem);	//按钮周围画虚线表示获得焦点
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//interesing