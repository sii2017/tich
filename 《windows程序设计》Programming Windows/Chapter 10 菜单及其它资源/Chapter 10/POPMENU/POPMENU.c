#include <windows.h>
#include "resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;
TCHAR szAppName[]= TEXT("PopMenu");

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
	wndclass.hIcon= LoadIcon(NULL, szAppName);	
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName= NULL;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("WRONG!"), szAppName, MB_ICONERROR);
		return 0;
	}

	
	hInst= hInstance;
	hwnd= CreateWindow(szAppName, TEXT("Popup Menu Demonstration"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);  //额外的补丁，来显示窗口
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HMENU hMenu;
	static int idColor[5]= {
		WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH
	};
	static int iSelection= IDM_BKGND_WHITE;
	POINT point;

	switch(message)
	{
	case WM_CREATE:
		hMenu= LoadMenu(hInst, szAppName);	//获取菜单的句柄
		hMenu= GetSubMenu(hMenu, 0);	//激活菜单，如果成功得到句柄，不成功则是NULL
		return 0;
	case WM_RBUTTONUP:	//在任意位置右键可以弹出菜单
		point.x= LOWORD(lParam);
		point.y= HIWORD(lParam);
		ClientToScreen(hwnd, &point);

		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y,0, hwnd, NULL);	//该函数在指定位置显示快捷菜单，并跟踪菜单项的选择
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
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
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);
			iSelection= LOWORD(wParam);
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);
			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)
				GetStockObject(idColor[LOWORD(wParam)-IDM_BKGND_WHITE]));
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case IDM_APP_ABOUT:
			MessageBox(hwnd, TEXT("Popup Menu Demonstration Program\n")TEXT("(c)Charles Petzold, 1998"),
				szAppName, MB_ICONINFORMATION|MB_OK);
			return 0;
		case IDM_APP_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		case IDM_APP_HELP:
			MessageBox(hwnd, TEXT("Help not yet implemented!"), szAppName, 
				MB_ICONEXCLAMATION|MB_OK);
			return 0;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//与之前的程序相似，不同的是菜单不再直接显示，而是通过右键的方式在右键的地点弹出竖列菜单。
//这里要注意的是，活的菜单句柄的方法，激活菜单句柄后再获得句柄，以及载入菜单的方式Track（该函数在指定位置显示快捷菜单，并跟踪菜单项的选择）