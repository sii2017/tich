#include <windows.h>
#include <commdlg.h>
#include "..\\emf8\\resource.h"

extern void CreateRoutine(HWND);
extern void PaintRoutine(HWND, HDC, int, int);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HANDLE hInst;
extern TCHAR szClass[];
extern TCHAR szTitle[];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	//正常创建，含有menu
	static TCHAR szResource[]= TEXT("EMF");
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
	wndclass.lpszMenuName= szResource;;
	wndclass.lpszClassName= szClass;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Regiseter Failed"),szResource, MB_ICONERROR) ;
		return 0;
	}

	hwnd = CreateWindow (szClass, szTitle,WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL) ;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//包含打印程序的函数
BOOL PrintRoutine(HWND hwnd)
{
	static DOCINFO di;
	static PRINTDLG printdlg= {sizeof(PRINTDLG)};
	static TCHAR szMessage[32];
	BOOL bSuccess= FALSE;
	HDC hdcPrn;
	int cxPage, cyPage;
	printdlg.Flags= PD_RETURNDC| PD_NOPAGENUMS| PD_NOSELECTION;
	if(!PrintDlg(&printdlg))
		return TRUE;
	if(NULL== (hdcPrn=printdlg.hDC))
		return FALSE;
	//hdcPrn是打印区域的句柄，这个大小是不会改变的，因此获得的尺寸也不会改变
	cxPage= GetDeviceCaps(hdcPrn, HORZRES);
	cyPage= GetDeviceCaps(hdcPrn, VERTRES);

	lstrcpy(szMessage, szClass);
	lstrcat(szMessage, TEXT(":Printing"));

	di.cbSize= sizeof(DOCINFO);
	di.lpszDocName= szMessage;

	if(StartDoc(hdcPrn, &di)>0)
	{
		if(StartPage(hdcPrn)>0)
		{
			PaintRoutine(hwnd, hdcPrn, cxPage, cyPage);	
			if(EndPage(hdcPrn)>0)
			{
				EndDoc(hdcPrn);
				bSuccess= TRUE;
			}
		}
	}
	DeleteDC(hdcPrn);
	return bSuccess;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess;
	static int cxClient, cyClient;
	HDC hdc;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CREATE:
		CreateRoutine(hwnd);//创建metafile图元文件emf8并保存在磁盘上。
		return 0;
	case WM_COMMAND:
		switch(wParam)
		{
		case IDM_PRINT:
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			bSuccess= PrintRoutine(hwnd);

			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if(!bSuccess)
				MessageBox(hwnd, TEXT("Error encountered during printing"), szClass,
				MB_ICONASTERISK| MB_OK);
			return 0;
		case IDM_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		case IDM_ABOUT:
			MessageBox(hwnd, TEXT("Enhanced MetaFile Demo Program\n"),
				szClass, MB_ICONINFORMATION|MB_OK);
			return 0;
		}
		break;
	case WM_SIZE:
		cxClient= LOWORD(lParam);
		cyClient= HIWORD(lParam);
		return 0;
	case WM_PAINT:
		hdc= BeginPaint(hwnd, &ps);
		PaintRoutine(hwnd, hdc, cxClient, cyClient);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}