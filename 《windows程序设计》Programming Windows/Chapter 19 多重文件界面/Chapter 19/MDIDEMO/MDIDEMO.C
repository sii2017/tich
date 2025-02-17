#include <windows.h>
#include "resource.h"

#define INIT_MENU_POS 0
#define HELLO_MENU_POS 2
#define RECT_MENU_POS 1 

#define IDM_FIRSTCHILD 50000

LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CloseEnumProc(HWND, LPARAM);
LRESULT CALLBACK HelloWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RectWndProc(HWND, UINT , WPARAM, LPARAM);

//Structure for storing data unique to each hello child window
typedef struct tagHELLODATA
{
	UINT iColor;
	COLORREF clrText;
}
HELLODATA, *PHELLODATA;

//Structure for storing data unique to each rect child window
typedef struct tagRECTDATA
{
	short cxClient;
	short cyClient;
}
RECTDATA, *PRECTDATA;

//global variables
TCHAR szAppName[]= TEXT("MDIDemo");
TCHAR szFrameClass[]= TEXT("MdiFrame");
TCHAR szHelloClass[]= TEXT("MdiHelloChild");
TCHAR szRectClass[]= TEXT("MdiRectChild");

HINSTANCE hInst;
HMENU hMenuInit, hMenuHello, hMenuRect;
HMENU hMenuInitWindow, hMenuHelloWindow, hMenuRectWindow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	HACCEL hAccel;
	HWND hwndFrame, hwndClient;
	MSG msg;
	WNDCLASS wndclass;

	hInst= hInstance;

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= FrameWndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= 0;
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)(COLOR_APPWORKSPACE+1);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szFrameClass;

	//注册第一个窗口框架窗口类
	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Register failed"), szAppName, MB_ICONERROR);
		return 0;
	}

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= HelloWndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= sizeof(HANDLE);
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szHelloClass;

	//注册hello子窗口窗口类
	RegisterClass(&wndclass);

	wndclass.style= CS_HREDRAW| CS_VREDRAW;
	wndclass.lpfnWndProc= RectWndProc;
	wndclass.cbClsExtra= 0;
	wndclass.cbWndExtra= sizeof(HANDLE);
	wndclass.hInstance= hInstance;
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szRectClass;

	//注册rect子窗口窗口类
	RegisterClass(&wndclass);

	//Obtain handles to three possible menu & submenus
	hMenuInit= LoadMenu(hInstance, TEXT("MdiMenuInit"));
	hMenuHello= LoadMenu(hInstance, TEXT("MdiMenuHello"));
	hMenuRect= LoadMenu(hInstance, TEXT("MdiMenuRect"));

	//分别获得三个menu状态下，根据windowmenu所在位置获得该句柄
	hMenuInitWindow= GetSubMenu(hMenuInit, INIT_MENU_POS);	//窗口框架的windowmenu在第一个位置，所以是0
	hMenuHelloWindow= GetSubMenu(hMenuHello, HELLO_MENU_POS);	//当打开hello后，hello的windowmenu在第三个位置，所以是2
	hMenuRectWindow= GetSubMenu(hMenuRect, RECT_MENU_POS);	//当打开rect后，hello的windowmenu在第二个位置，所以是1

	//Load accelerator table
	hAccel= LoadAccelerators(hInstance, szAppName);
	//Create the frame window
	hwndFrame= CreateWindow(szFrameClass, TEXT("MDI Demonstration"), 
		WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, hMenuInit, hInstance, NULL);
	hwndClient= GetWindow(hwndFrame, GW_CHILD);
	ShowWindow(hwndFrame, iCmdShow);
	UpdateWindow(hwndFrame);

	//Enter the modified message loop
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(!TranslateMDISysAccel(hwndClient, &msg)&&	//处理与指定MDI客户窗口相联系的多文档接口（MDI）子窗口的菜单命令的加速键响应。该函数转换WM_KEYUP和WM_KEYDOWN消息为WM_SYSCOMMAND消息，并把它的发送给相应MDI子窗口。
			!TranslateAccelerator(hwndFrame, hAccel, &msg))	//处理框架窗口的快捷键指令
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//Clean up by deleting unattched menus
	DestroyMenu(hMenuHello);
	DestroyMenu(hMenuRect);

	return msg.wParam;
}

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndClient;
	CLIENTCREATESTRUCT clientcreate;
	HWND hwndChild;
	MDICREATESTRUCT mdicreate;

	switch(message)
	{
	case WM_CREATE:
		clientcreate.hWindowMenu= hMenuInitWindow;
		clientcreate.idFirstChild= IDM_FIRSTCHILD;

		//这里以MDICLIENT窗口类建立了一个基础窗口，并且有一个菜单项可以调出rect和hello
		//由此可以看出上面的框架窗口，是真的只有一个最基础的框架而已。
		hwndClient= CreateWindow(TEXT("MDICLIENT"), NULL, 
			WS_CHILD| WS_CLIPCHILDREN| WS_VISIBLE, 
			0,0,0,0,hwnd, (HMENU)1, hInst, (PSTR)&clientcreate);
		return 0;
	case WM_COMMAND://实际上，这里并没有处理任何WM_COMMAND消息，即使看上去处理的部分也通过消息发送了出去，而其余的则发送给了子窗口进行处理
		switch(LOWORD(wParam))
		{
		case IDM_FILE_NEWHELLO:	//Crete a Hello child window
			mdicreate.szClass= szHelloClass;
			mdicreate.szTitle= TEXT("Hello");
			mdicreate.hOwner= hInst;
			mdicreate.x= CW_USEDEFAULT;
			mdicreate.y= CW_USEDEFAULT;
			mdicreate.cx= CW_USEDEFAULT;
			mdicreate.cy= CW_USEDEFAULT;
			mdicreate.style= 0;
			mdicreate.lParam= 0;
			hwndChild= (HWND)SendMessage(hwndClient, WM_MDICREATE, 0,
				(LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;
		case IDM_FILE_NEWRECT:	//Create a rect child window
			mdicreate.szClass= szRectClass;
			mdicreate.szTitle= TEXT("Rectangles");
			mdicreate.hOwner= hInst;
			mdicreate.x= CW_USEDEFAULT;
			mdicreate.y= CW_USEDEFAULT;
			mdicreate.cy= CW_USEDEFAULT;
			mdicreate.cy= CW_USEDEFAULT;
			mdicreate.style= 0;
			mdicreate.lParam= 0;

			hwndChild= (HWND)SendMessage(hwndClient, WM_MDICREATE, 0, 
				(LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;
		case IDM_FILE_CLOSE:	//关闭当前活动子窗口
			hwndChild= (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
			if(SendMessage(hwndChild, WM_QUERYENDSESSION, 0, 0))
				SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM)hwndChild, 0);
			return 0;
		case IDM_APP_EXIT:	//Exit the program
			SendMessage(hwnd, WM_CLOSE, 0,0 );
			return 0;
			//messages for arranging windows
		case IDM_WINDOW_TILE:
			SendMessage(hwndClient, WM_MDITILE, 0,0);
			return 0;
		case IDM_WINDOW_CASCADE:
			SendMessage(hwndClient, WM_MDICASCADE, 0,0);
			return 0;
		case IDM_WINDOW_ARRANGE:
			SendMessage(hwndClient, WM_MDIICONARRANGE, 0,0);
			return 0;
		case IDM_WINDOW_CLOSEALL:	//Attempt to close all children
			//传送一个引用CloseEnumProc函数的指标。此函数把WM_MDIRESTORE消息发送给每个子窗口，紧跟着发出WM_QUERYENDSESSION和WM_MDIDESTROY。
			EnumChildWindows(hwndClient, CloseEnumProc, 0);
			return 0;
		default:
			hwndChild= (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0,0);//获取子窗口的句柄
			if(IsWindow(hwndChild))
				SendMessage(hwndChild, WM_COMMAND, wParam, lParam);	//将所有其它菜单中的command消息发送给子窗口让他们自己处理
			break;	//... and then to DefFrameProc
		}
		break;
	case WM_QUERYENDSESSION:
	case WM_CLOSE:	//Attempt to close all children
		SendMessage(hwnd, WM_COMMAND, IDM_WINDOW_CLOSEALL, 0);

		if(NULL!= GetWindow(hwndClient,  GW_CHILD))
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefFrameProc(hwnd, hwndClient, message, wParam, lParam);
}

BOOL CALLBACK CloseEnumProc(HWND hwnd, LPARAM lParam)
{
	if(GetWindow(hwnd, GW_OWNER))	//Check for icon title
		return TRUE;
	SendMessage(GetParent(hwnd), WM_MDIRESTORE, (WPARAM)hwnd, 0);
	if(!SendMessage(hwnd, WM_QUERYENDSESSION, 0, 0))
		return TRUE;
	SendMessage(GetParent(hwnd), WM_MDIDESTROY, (WPARAM)hwnd, 0);
	return TRUE;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COLORREF clrTextArray[]= {
		RGB(0,0,0), RGB(255,0,0), RGB(0,255,0), RGB(0,0,255), RGB(255,255,255)
	};
	static HWND hwndClient, hwndFrame;
	HDC hdc;
	HMENU hMenu;
	PHELLODATA pHelloData;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		//申请内存并且初始化
		pHelloData= (PHELLODATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
			sizeof(HELLODATA));
		pHelloData->iColor= IDM_COLOR_BLACK;
		pHelloData->clrText= RGB(0,0,0);
		SetWindowLong(hwnd, 0, (long)pHelloData);	//从第0个字节开始将pHelloData放入之前预留的额外空间

		//Save some window handles
		hwndClient= GetParent(hwnd);
		hwndFrame= GetParent(hwndClient);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_COLOR_BLACK:
		case IDM_COLOR_RED:
		case IDM_COLOR_GREEN:
		case IDM_COLOR_BLUE:
		case IDM_COLOR_WHITE:
			//Change the text color
			pHelloData= (PHELLODATA)GetWindowLong(hwnd, 0);

			hMenu= GetMenu(hwndFrame);

			CheckMenuItem(hMenu, pHelloData->iColor, MF_UNCHECKED);
			pHelloData->iColor= wParam;
			CheckMenuItem(hMenu, pHelloData->iColor, MF_CHECKED);

			pHelloData->clrText= clrTextArray[wParam- IDM_COLOR_BLACK];

			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;
	case WM_PAINT:
		//Paint the window
		hdc= BeginPaint(hwnd, &ps);
		pHelloData= (PHELLODATA)GetWindowLong(hwnd, 0);
		SetTextColor(hdc, pHelloData->clrText);
		GetClientRect(hwnd, &rect);

		DrawText(hdc, TEXT("Hello, World!"), -1, &rect, 
			DT_SINGLELINE| DT_CENTER| DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_MDIACTIVATE://wParam和lParam值分别是失去活动和被启动窗口的句柄
		//Set the Hello menu if gaining focus
		if(lParam== (LPARAM)hwnd)
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuHello, 
			(LPARAM)hMenuHelloWindow);
		//Check or uncheck menu item
		pHelloData= (PHELLODATA)GetWindowLong(hwnd, 0);
		CheckMenuItem(hMenuHello, pHelloData->iColor, 
			(lParam== (LPARAM)hwnd)? MF_CHECKED: MF_UNCHECKED);
		//Set the Init menu if losing focus 
		if(lParam!= (LPARAM)hwnd)
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit, (LPARAM)hMenuInitWindow);

		DrawMenuBar(hwndFrame);
		return 0;
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if(IDOK!= MessageBox(hwnd, TEXT("OK to close window?"), TEXT("Hello"),
			MB_ICONQUESTION| MB_OKCANCEL))
			return 0;
		break;
	case WM_DESTROY:
		pHelloData= (PHELLODATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pHelloData);	//释放窗口类额外配置的内存空间
		return 0;
	}
	//Pass unprocessed message to DefMDChildProc
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK RectWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndClient, hwndFrame;
	HBRUSH hBrush;
	HDC hdc;
	PRECTDATA pRectData;
	PAINTSTRUCT ps;
	int xLeft, xRight, yTop, yBottom;
	short nRed, nGreen, nBlue;

	switch(message)
	{
	case WM_CREATE:
		//Allocate memory for window private data
		pRectData= (PRECTDATA)HeapAlloc(GetProcessHeap(), 
			HEAP_ZERO_MEMORY, sizeof(RECTDATA));
		SetWindowLong(hwnd, 0, (long)pRectData);
		//Start the timer going
		SetTimer(hwnd, 1, 250, NULL);
		//Save some window handles
		hwndClient= GetParent(hwnd);
		hwndFrame= GetParent(hwndClient);
		return 0;
	case WM_SIZE:
		//if not minimized, save the window size
		if(wParam!= SIZE_MINIMIZED)
		{
			pRectData= (PRECTDATA)GetWindowLong(hwnd, 0);
			pRectData->cxClient= LOWORD(lParam);
			pRectData->cyClient= HIWORD(lParam);
		}
		break;
	case WM_TIMER:
		//Display a random rectangle
		pRectData= (PRECTDATA)GetWindowLong(hwnd, 0);
		xLeft= rand()%pRectData->cxClient;
		xRight= rand()%pRectData->cxClient;
		yTop= rand()%pRectData->cyClient;
		yBottom= rand()%pRectData->cyClient;
		nRed= rand()&255;
		nGreen= rand()&255;
		nBlue= rand()&255;

		hdc= GetDC(hwnd);
		hBrush= CreateSolidBrush(RGB(nRed, nGreen, nBlue));
		SelectObject(hdc, hBrush);
		Rectangle(hdc, min(xLeft, xRight), min(yTop, yBottom), 
			max(xLeft, xRight), max(yTop, yBottom));
		ReleaseDC(hwnd, hdc);
		DeleteObject(hBrush);
		return 0;
	case WM_PAINT:	//Clear the window
		InvalidateRect(hwnd, NULL, TRUE);
		hdc= BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_MDIACTIVATE:	//Set the appropriate menu
		if(lParam== (LPARAM)hwnd)
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuRect, (LPARAM)hMenuRectWindow);
		else
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit, (LPARAM)hMenuInitWindow);
		DrawMenuBar(hwndFrame);
		return 0;
	case WM_DESTROY:
		pRectData= (PRECTDATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pRectData);
		KillTimer(hwnd, 1);
		return 0;
	}
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}