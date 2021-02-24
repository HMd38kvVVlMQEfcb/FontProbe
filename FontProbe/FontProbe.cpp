#define _WIN32_WINNT 0x501
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <strsafe.h>
#include <stdlib.h>
#include "Resource.h"


#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


static ATOM                RegisterClass_MainWindow();
static LRESULT CALLBACK    WndProc_MainWindow(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
static void                DrawExamples(HWND hWnd, HDC hdc, bool ForceClearType);
static void                DrawFontInfo(HDC hdc, LPCWSTR Name, LOGFONTW *LogFont, int *y, bool ForceClearType);
static BOOL                ResolveFontSubstitute(LOGFONTW *LogFont);
static INT_PTR CALLBACK    DlgProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


static HINSTANCE hInst;
static HWND DonorDialog;
static HWND DonorDialog2;


int APIENTRY wWinMain(HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPWSTR CmdLine,
             int nCmdShow)
{
	hInst = hInstance;
	ATOM Atom_MainWindow = RegisterClass_MainWindow();
	HWND MainWindow = CreateWindowExW(0, MAKEINTATOM(Atom_MainWindow), L"FontProbe", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);
	ShowWindow(MainWindow, nCmdShow);

	MSG Msg = {};
	while (GetMessageW(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}

	return (int)Msg.wParam;
}


static ATOM RegisterClass_MainWindow()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = 0;
	wcex.lpfnWndProc    = WndProc_MainWindow;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInst;
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = L"FontProbeMainWindow";
	wcex.hIconSm        = NULL;

	return RegisterClassExW(&wcex);
}


static LRESULT CALLBACK WndProc_MainWindow(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_F5:
				{
					DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, DlgProc_About);
					break;
				}
				case VK_F6:
				{
					DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX2), hWnd, DlgProc_About);
					break;
				}
			}
			return 0;
		}

		case WM_ERASEBKGND:
		{
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps = {};
			HDC hdc = BeginPaint(hWnd, &ps);

			if (ps.fErase)
			{
				FillRect(hdc, &ps.rcPaint, (HBRUSH)GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND));
			}

			DrawExamples(hWnd, hdc, false);
			SetViewportOrgEx(hdc, 340, 0, NULL);
			DrawExamples(hWnd, hdc, true);

			EndPaint(hWnd, &ps);

			return 0;
		}
	}

	return DefWindowProcW(hWnd, Message, wParam, lParam);
}


static void DrawExamples(HWND hWnd, HDC hdc, bool ForceClearType)
{
	// Cursor for drawing text
	int y = 0;

	//
	// DEFAULT_GUI_FONT
	//

	{
		HFONT StockDefaultGuiFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SelectObject(hdc, StockDefaultGuiFont);
		LOGFONT LogFont = {};
		GetObject(StockDefaultGuiFont, sizeof(LogFont), &LogFont);
		DrawFontInfo(hdc, L"DEFAULT_GUI_FONT", &LogFont, &y, ForceClearType);
	}

	y += 10;

	//
	// MS Shell Dlg 2 8pt
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = -MulDiv(8, dpi, 72);
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"MS Shell Dlg 2");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"MS Shell Dlg 2", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}
	
	y += 10;

	//
	// MS Shell Dlg 2 9pt
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = -MulDiv(9, dpi, 72);
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"MS Shell Dlg 2");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"MS Shell Dlg 2", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}
	
	y += 10;

	//
	// SPI_NONCLIENTMETRICS
	//

	{
		NONCLIENTMETRICSW NonClientMetrics = {};
		NonClientMetrics.cbSize = sizeof(NonClientMetrics);
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NonClientMetrics), &NonClientMetrics, 0);

		LOGFONTW LogFont = NonClientMetrics.lfMessageFont;
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"SPI_NONCLIENTMETRICS (lfMessageFont)", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}
	
	y += 10;

	//
	// Steal from About Dialog
	//

	{
		if (DonorDialog == NULL)
		{
			DonorDialog = CreateDialogW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, DlgProc_About);
		}
		HFONT Font = (HFONT)SendMessageW(DonorDialog, WM_GETFONT, 0, 0);
		SelectObject(hdc, Font);

		LOGFONT LogFont = {};
		GetObject(Font, sizeof(LogFont), &LogFont);
		
		DrawFontInfo(hdc, L"AboutBox, MS Shell Dlg, 8 DS_SETFONT", &LogFont, &y, ForceClearType);
	}

	y += 10;

	//
	// Steal from About Dialog (DS_SHELLFONT)
	//

	{
		if (DonorDialog2 == NULL)
		{
			DonorDialog2 = CreateDialogW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX2), hWnd, DlgProc_About);
		}
		HFONT Font = (HFONT)SendMessageW(DonorDialog2, WM_GETFONT, 0, 0);
		SelectObject(hdc, Font);

		LOGFONT LogFont = {};
		GetObject(Font, sizeof(LogFont), &LogFont);
		
		DrawFontInfo(hdc, L"AboutBox, MS Shell Dlg, 8 DS_SHELLFONT (!)", &LogFont, &y, ForceClearType);
	}

	y += 10;

	//
	// MS Sans Serif
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = -MulDiv(8, dpi, 72);
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"MS Sans Serif");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"MS Sans Serif", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}

	//
	// Microsoft Sans Serif
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = -MulDiv(8, dpi, 72);
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"Microsoft Sans Serif");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"Microsoft Sans Serif", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}

	//
	// Tahoma
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = -MulDiv(8, dpi, 72);
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"Tahoma");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"Tahoma", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}

	y += 10;

	//
	// MS UI Gothic, +12px
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = 12;
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"MS UI Gothic");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"MS UI Gothic 12", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}

	//
	// MS UI Gothic, +13px
	//

	{
		
		LOGFONT LogFont = {};
		LogFont.lfCharSet = DEFAULT_CHARSET;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		LogFont.lfHeight = 13;
		StringCchCopyW(LogFont.lfFaceName, _countof(LogFont.lfFaceName), L"MS UI Gothic");
		HFONT Font = CreateFontIndirectW(&LogFont);
		SelectObject(hdc, Font);
		DrawFontInfo(hdc, L"MS UI Gothic 13", &LogFont, &y, ForceClearType);
		DeleteObject(Font);
	}
}


static void DrawFontInfo(HDC hdc, LPCWSTR Name, LOGFONTW *LogFont, int *y, bool ForceClearType)
{
	HFONT ClearTypeFont = NULL;
	HGDIOBJ PreviousFont = NULL;

	// If ForceClearType is false, we expect that the font is already selected into the device.
	// Otherwise, we create a temporary font.
	if (ForceClearType)
	{
		LogFont->lfQuality = CLEARTYPE_QUALITY;
		ClearTypeFont = CreateFontIndirectW(LogFont);
		PreviousFont = SelectObject(hdc, ClearTypeFont);
	}

	HFONT CurrentFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
	LOGFONTW LogFontRead = {};
	GetObject(CurrentFont, sizeof(LogFontRead), &LogFontRead);

	TEXTMETRICW TextMetrics = {};
	GetTextMetricsW(hdc, &TextMetrics);

	RECT rc = {};
	rc.top = *y;
	WCHAR buf[100];
	LPWSTR str = buf;
	size_t remaining = _countof(buf);
	StringCchCopyExW(str, remaining, Name, &str, &remaining, 0);
	StringCchCopyExW(str, remaining, L": naEI,1g", &str, &remaining, 0);
	*y += DrawTextW(hdc, buf, -1, &rc, DT_NOCLIP | DT_NOPREFIX);
	rc.top = *y;
	*y += DrawTextW(hdc, L"\u65e5\u672c\u6587\u304b\u306a\u30ab\u30ca\u30e1\u30d5\u8272\u56de\u7121", -1, &rc, DT_NOCLIP | DT_NOPREFIX);
	rc.top = *y;
	StringCchPrintfW(buf, _countof(buf), L"%s   (%d; %d)", LogFont->lfFaceName, LogFontRead.lfHeight, TextMetrics.tmHeight);
	*y += DrawTextW(hdc, buf, -1, &rc, DT_NOCLIP | DT_NOPREFIX);
	LOGFONTW LogFontResolved = *LogFont;
	if (ResolveFontSubstitute(&LogFontResolved))
	{
		rc.top = *y;
		*y += DrawTextW(hdc, L"Substitution resolved to:", -1, &rc, DT_NOCLIP | DT_NOPREFIX);
		rc.top = *y;
		*y += DrawTextW(hdc, LogFontResolved.lfFaceName, -1, &rc, DT_NOCLIP | DT_NOPREFIX);
	}

	if (PreviousFont != NULL)
	{
		SelectObject(hdc, PreviousFont);
	}
	if (ClearTypeFont != NULL)
	{
		DeleteObject(ClearTypeFont);
	}
}


// Check the FontSubstitutes registry key to update the name of the logfont.
static BOOL ResolveFontSubstitute(LOGFONTW *LogFont)
{
	// It turns out that this also happens to be what GDI+ does internally in the function "CacheFontSubstitutionDataW" when it creates
	// GDI+ Font objects (although it's a bit more intelligent about caching this stuff).
	// The registry key is hard-coded in there as well, so we're probably on the right track doing this here.
	HKEY KeyFontSubstitutes = NULL;
	if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes", 0, KEY_QUERY_VALUE, &KeyFontSubstitutes))
	{
		return false;
	}

	bool Success = false;
	WCHAR Substitution[100];
	DWORD SubstitutionSize = sizeof(Substitution);
	DWORD ValueType = 0;
	if (ERROR_SUCCESS == RegQueryValueExW(KeyFontSubstitutes, LogFont->lfFaceName, 0, &ValueType, (BYTE *)Substitution, &SubstitutionSize))
	{
		if (ValueType == REG_SZ && SubstitutionSize != 0)
		{
			StringCchCopyW(LogFont->lfFaceName, _countof(LogFont->lfFaceName), Substitution);
			Success = true;
		}
	}
	
	RegCloseKey(KeyFontSubstitutes);
	return Success;
}


static INT_PTR CALLBACK DlgProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
		{
			// Idk how to specify unicode strings in a resource file. Just putting in this string as it is doesn't work...
			LPCWSTR RealLabelText = L"Version 1.0 \u65e5\u672c\u6587\u304b\u306a\u30ab\u30ca\u30e1\u30d5\u8272\u56de\u7121";
			HWND Label = GetDlgItem(hDlg, IDC_LABEL);
			SetWindowTextW(Label, RealLabelText);
			return (INT_PTR)TRUE;
		}

		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
