/*
这并不是一个传统意义上的写字板，只是我们可以通过黏贴将剪贴板里的字通过DrawText来显示到程序中
*/

#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#ifdef UNICODE 
#define CF_TCHAR CF_UNICODETEXT
TCHAR szDefaultText[]= TEXT("Default Text- Unicode Version");
TCHAR szCaption[]= TEXT("Clipboard Text Transfers- Unicode Version");
#else
#define CF_TCHAR CF_TEXT
TCHAR szDefaultText[]= TEXT("Default Text- ANSI Version");
TCHAR szCaption[]= TEXT("Clipboard Text Transfers- ANSI Version");
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("ClipText");
	HACCEL hAccel;
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
	wndclass.lpszMenuName= szAppName;
	wndclass.lpszClassName= szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Regist Wrong"), szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd= CreateWindow(szAppName, szCaption, WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	hAccel= LoadAccelerators(hInstance, szAppName);

	while(GetMessage(&msg, NULL, 0,0))
	{
		if(!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static PTSTR pText;
	BOOL bEnable;
	HGLOBAL hGlobal;
	HDC hdc;
	PTSTR pGlobal;
	PAINTSTRUCT ps;
	RECT rect;

	switch(message)
	{
	case WM_CREATE:
		SendMessage(hwnd, WM_COMMAND, IDM_EDIT_RESET, 0);
		return 0;
	case WM_INITMENUPOPUP:
		//查看剪贴簿里是否有内容觉得黏贴按钮是否亮
		EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, 
			IsClipboardFormatAvailable(CF_TCHAR)?MF_ENABLED:MF_GRAYED);
		bEnable= pText?MF_ENABLED:MF_GRAYED;

		EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, bEnable);
		EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, bEnable);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_EDIT_PASTE:
			OpenClipboard(hwnd);
			if(hGlobal= GetClipboardData(CF_TCHAR))
			{
				pGlobal= GlobalLock(hGlobal);
				if(pText)
				{
					free(pText);
					pText= NULL;
				}
				pText= malloc(GlobalSize(hGlobal));
				lstrcpy(pText, pGlobal);
				InvalidateRect(hwnd, NULL, TRUE);
			}
			CloseClipboard();
			return 0;
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
			if(!pText)	//如果pText没有内容则return
				return 0;
			//申请一块一样大小的内存，将pText里的内容复制下来
			hGlobal= GlobalAlloc(GHND| GMEM_SHARE, 
				(lstrlen(pText)+1)* sizeof(TCHAR));
			pGlobal= GlobalLock(hGlobal);
			lstrcpy(pGlobal, pText);
			GlobalUnlock(hGlobal);

			OpenClipboard(hwnd);
			EmptyClipboard();
			SetClipboardData(CF_TCHAR, hGlobal);//将内存存入给到剪贴板
			CloseClipboard();

			//如果是复制则结束，如果是剪贴则继续下去把内容删除
			if(LOWORD(wParam)==IDM_EDIT_COPY)
				return 0;
		case IDM_EDIT_CLEAR:
			if(pText)
			{
				free(pText);
				pText= NULL;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case IDM_EDIT_RESET:
			if(pText)
			{
				free(pText);
				pText= NULL;
			}
			pText=  malloc((lstrlen(szDefaultText)+1)* sizeof(TCHAR));
			lstrcpy(pText, szDefaultText);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		if(pText!=NULL)
			DrawText(hdc, pText, -1, &rect, DT_EXPANDTABS| DT_WORDBREAK);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if(pText)
			free(pText);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,message, wParam, lParam);
}