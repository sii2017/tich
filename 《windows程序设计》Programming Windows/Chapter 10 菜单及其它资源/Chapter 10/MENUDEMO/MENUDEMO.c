#include <windows.h>
#include "resource.h"
#define ID_TIMER 1
HMENU hMenu1;	//额外的补丁，来显示窗口
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szAppName[]= TEXT("MenuDemo");
TCHAR szText[]= TEXT("IDM_MENUDEMO");	//额外的补丁，来显示窗口

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
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
	wndclass.hbrBackground= GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= szAppName;	//书上使用这个方法进行添加，但是失败。网上也有很多人都失败了。书上肯定漏了一些东西没有讲。
	//wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}

	
	hMenu1= LoadMenu (hInstance, MAKEINTRESOURCE (IDM_MENUDEMO)); 

	hwnd= CreateWindow(szAppName, TEXT("Menu Demonstration"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, /*hMenu1*/NULL, hInstance, NULL);  //额外的补丁，来显示窗口
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int idColor[5]= {
		WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH
	};
	static int iSelection= IDM_BKGND_WHITE;
	HMENU hMenu;

	switch(message)
	{
	case WM_COMMAND:
		hMenu= GetMenu(hwnd);	//这里通过hwnd能直接获取menu？
		switch(LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
			MessageBeep(0);
			return 0;
		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		case IDM_EDIT_UNDO:
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
			MessageBeep(0);
			return 0;
		case IDM_BKGND_WHITE:
		case IDM_BKGND_LTGRAY:
		case IDM_BKGND_GRAY:
		case IDM_BKGND_DKGRAY:
		case IDM_BKGND_BLACK:
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);//将原来的check取消
			iSelection= LOWORD(wParam);//新的背景颜色从低字节获取
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);//选中新的项目
			SetClassLong(hwnd, GCL_HBRBACKGROUND, 
				(LONG)GetStockObject(idColor[LOWORD(wParam)-IDM_BKGND_WHITE]));//刷新背景颜色
			InvalidateRect(hwnd, NULL, TRUE);//重新刷新页面
			return 0;
		case IDM_TIMER_START:
			//设置定时器，如果成功则无效化start，有效化stop
			if(SetTimer(hwnd, ID_TIMER, 1000, NULL))
			{
				EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);
			}
			return 0;
		case IDM_TIMER_STOP:
			KillTimer(hwnd, ID_TIMER);
			EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);
			return 0;
		case IDM_APP_HELP:
			MessageBox(hwnd, TEXT("Help not yet implemented!"),
				szAppName, MB_ICONEXCLAMATION|MB_OK);
			return 0;
		case IDM_APP_ABOUT:
			MessageBox(hwnd, TEXT("Menu Demonstration Program\n")TEXT("(c)cHARLES pETZOLD, 1998"), 
				szAppName, MB_ICONINFORMATION|MB_OK);
			return 0;
		}
		break;
	case WM_TIMER:
		MessageBeep(0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//这个程序中介绍了一些系统菜单和自定义菜单的消息，进行一些处理，亦或者是通过蜂鸣声来代替
//这里碰到了一个问题，当我们通过wnd来设置menu的时候会失败，通过查询网上失败了非常多的人。
//解决方法有很多，但是目前无一成功，为了进度，暂时不进行无所谓的尝试，等下个程序再看
//现在用了第二种方法既在CreateWindow中放入menu句柄来加载menu