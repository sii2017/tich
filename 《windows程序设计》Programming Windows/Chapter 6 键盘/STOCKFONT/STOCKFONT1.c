#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[]= TEXT("StokFont");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style= CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc= WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance= hInstance;
	wndclass.lpszClassName= szAppName;
	wndclass.lpszMenuName= NULL;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("wrong~"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd= CreateWindow(szAppName, TEXT("Stock Font"), WS_OVERLAPPEDWINDOW|WS_VSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static struct
	{
		int idStockFont;
		TCHAR* szStockFont;	//字符用指针好还是数组好
	}
	stockfont[]= {
		OEM_FIXED_FONT, "OEM_FIXED_FONT",
		ANSI_FIXED_FONT, "ANSI_FIXED_FONT",
		ANSI_VAR_FONT, "ANSI_VAR_FONT",
		SYSTEM_FONT, "SYSTEM_FONT",
		DEVICE_DEFAULT_FONT, "DEVICE_DEFAULT_FONT",
		SYSTEM_FIXED_FONT, "SYSTEM_FIXED_FONT",
		DEFAULT_GUI_FONT, "DEFAULT_GUI_FONT"};

		static int iFont, cFonts= sizeof stockfont/ sizeof stockfont[0];
		HDC hdc;
		int i,x,y,cxGrid, cyGrid;
		PAINTSTRUCT ps;
		TCHAR szFaceName[LF_FACESIZE], szBuffer[LF_FACESIZE+64];
		TEXTMETRIC tm;
		switch(message)
		{
		case WM_CREATE:
			SetScrollRange(hwnd, SB_VERT, 0, cFonts-1, TRUE);//设计一个滚动条，这里的滚动条不是用来滚动页面而是用来转换不同的文字。
			return 0;
		case WM_DISPLAYCHANGE:
			InvalidateRect(hwnd, NULL, TRUE);//如果分辨率改变则重新使画面无效使之刷新
			return 0;
		case WM_VSCROLL:
			switch(LOWORD(wParam))
			{
			case SB_TOP:
				iFont= 0;
				break;
			case SB_BOTTOM:
				iFont= cFonts-1;
				break;
			case SB_LINEDOWN:
			case SB_PAGEDOWN:
				iFont += 1;
				break;
			case SB_THUMBPOSITION:
				iFont= HIWORD(wParam);
				break;
			}
			iFont= max(0, min(cFonts-1, iFont));
			SetScrollPos(hwnd, SB_VERT, iFont, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case WM_KEYDOWN://对一些滚动相关的按键进行处理
			switch(wParam)
			{
			case VK_HOME: SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);break;
			case VK_END: SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);break;
			case VK_PRIOR:
			case VK_LEFT:
			case VK_UP:
				SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
				break;
			case VK_NEXT:
			case VK_RIGHT:
			case VK_DOWN:
				SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
				break;
			}
			return 0;
		case WM_PAINT:
			hdc= BeginPaint(hwnd, &ps);
			SelectObject(hdc, GetStockObject(stockfont[iFont].idStockFont));
			GetTextFace(hdc, LF_FACESIZE, szFaceName);//得到字体样式名称
			GetTextMetrics(hdc, &tm);
			cxGrid= max(3*tm.tmAveCharWidth, 2*tm.tmMaxCharWidth);
			cyGrid= tm.tmHeight +3;

			TextOut(hdc, 0, 0, szBuffer, 
				wsprintf(szBuffer, TEXT("%s: Face Name= %s, CharSet= %i"),
				stockfont[iFont].szStockFont, szFaceName, tm.tmCharSet));	//由于版本问题szStockFont显示出的为乱码
			SetTextAlign(hdc, TA_TOP|TA_CENTER);
			//vertical and horizontal lines
			for(i=0;i<17;i++)
			{
				MoveToEx(hdc, (i+2)*cxGrid, 2*cyGrid, NULL);
				LineTo(hdc, (i+2)*cxGrid, 19*cyGrid);

				MoveToEx(hdc, cxGrid, (i+3)*cyGrid, NULL);
				LineTo(hdc, 18*cxGrid, (i+3)*cyGrid);
			}
			//vertical and horizontal headings
			for(i=0;i<16;i++)
			{
				TextOut(hdc, (2*i+5)*cxGrid/2, 2*cyGrid+2, szBuffer, 
					wsprintf(szBuffer, TEXT("%X-"), i));
				TextOut(hdc, 3*cxGrid/2, (i+3)*cyGrid+2, szBuffer,
					wsprintf(szBuffer, TEXT("-%X"), i));
			}
			//characters
			for(y=0;y<16;y++)
				for(x=0;x<16;x++)
			{
				TextOut(hdc, (2*x+5)*cxGrid/2,
					(y+3)*cyGrid+2, szBuffer,
					wsprintf(szBuffer, TEXT("%c"), 16*x+y));
			}
			EndPaint(hwnd, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
}