
#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <CommDlg.h>
#include "OSiSP_Lab1.h"


#define MAX_LOADSTRING 100
#define ID_EDIT 1006
#define CM_FILE_OPEN 1007
#define CM_FILE_SAVE 1008
#define ID_BUTTONCOLORPEN 1009
#define ID_BUTTONPRINT 1012
#define ID_BUTTONNEW 1013
#define ID_BUTTONWIDTH 1014
#define ID_BUTTONWIDTH1 1015
#define ID_BUTTONWIDTH2 1016
#define ID_BUTTONWIDTH3 1017
#define ID_BUTTONWIDTH4 1018
#define ID_BUTTONWIDTH5 1019
#define ID_BUTTONWIDTH6 1020
#define ID_BUTTONCOLORBRUSH 1021
#define ID_BUTTONCOLOR 1022
#define ID_BUTTONCOLORNONE 1023
#define ID_BUTTONCOLORFILL 1024
#define ID_BUTTONABOUT 1025
#define ID_BUTTONLINE 1050
#define ID_BUTTTONRECTANGLE 1051
#define ID_BUTTONELLIPSE 1052
#define ID_BUTTONPOLYGON 1053
#define ID_BUTTONPOLYLINE 1054
#define ID_BUTTONTEXT 1055
#define ID_BUTTONPENCIL 1056
#define ID_BUTTONZOOM 1057
#include <string>


// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна


// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OSISP_LAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSISP_LAB1));

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSISP_LAB1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= HBRUSH(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OSISP_LAB1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle,  WS_MAXIMIZE|WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_MAXIMIZE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
int penWidth = 0;

int wmId, wmEvent;
PAINTSTRUCT ps;

HDC screenDC, backDC, frontDC, tmpDC;
HBITMAP backBit, frontBit, tmpBit;
HANDLE tmpHandle;

int coordXPolyStart, coordYPolyStart,
	xCoordShapeStart, yCoordShapeStart,
	xCoordShapeEnd, yCoordShapeEnd, 
	xBegin, yBegin;

double scale = 1;
int zDelta;

bool isLeftMouseDown, isTextPrinting, isPolyDrawing;

std::wstring text;
int xTextCoord, yTextCoord;

CHOOSECOLOR penColorChooser, brushColorChooser;
COLORREF  crCustColor[16];

PRINTDLG printDialog;
DOCINFO printDocInfo;

HBRUSH brush;
HPEN pen;

RECT rect;
int I, checkedInstrumentId;

static OPENFILENAME hMetaFileDlgBox;
static WCHAR fullpath[256], filename[256], dir[256];


void ErrorExit(HWND hWnd, LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		LPTSTR(&lpMsgBuf),
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = LPVOID(LocalAlloc(LMEM_ZEROINIT, (lstrlen(LPCTSTR(lpMsgBuf)) 
		+ lstrlen(LPCTSTR(lpszFunction)) + 40) * sizeof(TCHAR)));
	StringCchPrintf(LPTSTR(lpDisplayBuf),
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
}


void FillWhiteRectangle(HDC dc, RECT rect)
{
	int dcNum = SaveDC(dc);
	DeleteObject(SelectObject(dc, HBRUSH(WHITE_BRUSH)));
	PatBlt(dc, 0, 0, rect.right, rect.bottom, PATCOPY);
	RestoreDC(dc, dcNum);
}

BOOL CreateMenuItem(HMENU hMenu, WCHAR *str, UINT uIns, UINT uCom, HMENU hSubMenu, BOOL flag, UINT fType)
{
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
	mii.fType = fType;
	mii.fState = MFS_ENABLED;
	mii.dwTypeData = str;
	mii.cch = sizeof(str);
	mii.wID = uCom;
	mii.hSubMenu = hSubMenu;
	return InsertMenuItem(hMenu, uIns, flag, &mii);
}

void AddWndMenu(HWND hWnd)
{
	static HMENU hMainMenu, hFileMenu, hInstrumentMenu, hAboutMenu,
		hActionMenu, hButtonWidth, hButtonColor, hButtonColorBrush;
	static int i;

	hMainMenu = CreateMenu();
	hFileMenu = CreatePopupMenu();
	hInstrumentMenu = CreatePopupMenu();
	hActionMenu = CreatePopupMenu();
	hButtonWidth = CreatePopupMenu();
	hButtonColor = CreatePopupMenu();
	hButtonColorBrush = CreatePopupMenu();
	hAboutMenu = CreatePopupMenu();

	i = 0;
	CreateMenuItem(hFileMenu, L"&New", i++, ID_BUTTONNEW, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hFileMenu, L"&Open", i++, CM_FILE_OPEN, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hFileMenu, L"&Save as", i++, CM_FILE_SAVE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hFileMenu, L"&Print", i++, ID_BUTTONPRINT, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hInstrumentMenu, L"&Pencil", i++, ID_BUTTONPENCIL, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Line", i++, ID_BUTTONLINE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Rectangle", i++, ID_BUTTTONRECTANGLE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Ellipse", i++, ID_BUTTONELLIPSE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Polyline", i++, ID_BUTTONPOLYLINE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Polygon", i++, ID_BUTTONPOLYGON, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hInstrumentMenu, L"&Text", i++, ID_BUTTONTEXT, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hButtonWidth, L"&1", i++, ID_BUTTONWIDTH1, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonWidth, L"&2", i++, ID_BUTTONWIDTH2, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonWidth, L"&3", i++, ID_BUTTONWIDTH3, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonWidth, L"&4", i++, ID_BUTTONWIDTH4, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonWidth, L"&5", i++, ID_BUTTONWIDTH5, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonWidth, L"&6", i++, ID_BUTTONWIDTH6, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hActionMenu, L"&Zoom", i++, ID_BUTTONZOOM, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hButtonColorBrush, L"&None", i++, ID_BUTTONCOLORNONE, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonColorBrush, L"&Fill", i++, ID_BUTTONCOLORFILL, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hButtonColor, L"&Pen", i++, ID_BUTTONCOLORPEN, NULL, FALSE, MFT_STRING);
	CreateMenuItem(hButtonColor, L"&Brush", i++, 0, hButtonColorBrush, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hAboutMenu, L"&About", i++, ID_BUTTONABOUT, NULL, FALSE, MFT_STRING);
	i = 0;
	CreateMenuItem(hMainMenu, L"&File", i++, 0, hFileMenu, FALSE, MFT_STRING);
	CreateMenuItem(hMainMenu, L"&Shape", i++, 0, hInstrumentMenu, FALSE, MFT_STRING);
	CreateMenuItem(hMainMenu, L"&Zoom", i++, 0, hActionMenu, FALSE, MFT_STRING);
	CreateMenuItem(hMainMenu, L"&Color", i++, 0, hButtonColor, FALSE, MFT_STRING);
	CreateMenuItem(hMainMenu, L"&Width", i++, 0, hButtonWidth, FALSE, MFT_STRING);
	CreateMenuItem(hMainMenu, L"&About", i++, 0, hAboutMenu, FALSE, MFT_STRING);
	SetMenu(hWnd, hMainMenu);
	DrawMenuBar(hWnd);
}

bool IsInstrumentId(int id)
{
	return id >= 1050 && id <= 1060;
}

void DeactivateScaleAndSaveInstrumentId(HWND hWnd, int instrumentId)
{
	if (checkedInstrumentId != instrumentId)
	{
		isPolyDrawing = false;
		isTextPrinting = false;
		scale = 1;
		xBegin = 0;
		yBegin = 0;
		checkedInstrumentId = instrumentId;		
	}
	//InvalidateRect(hWnd, NULL, FALSE);
}

// Selects handle (pen or brush and so on) for All DCs
void SelectHandleForAllDCs(HANDLE handle)
{
	DeleteObject(SelectObject(frontDC, handle));
	DeleteObject(SelectObject(backDC, handle));
	//DeleteObject(SelectObject(fileDC, handle));
}

void ChangePen(int width)
{
	penWidth = width;
	DeleteObject(pen);
	pen = CreatePen(PS_SOLID, penWidth, penColorChooser.rgbResult);
	SelectHandleForAllDCs(pen);
}

void FromBackDCToFrontDC(HWND hWnd)
{
	tmpDC = GetDC(hWnd);
	frontBit = CreateCompatibleBitmap(tmpDC, rect.right, rect.bottom);
	DeleteObject(SelectObject(frontDC, frontBit));
	BitBlt(frontDC, 0, 0, rect.right, rect.bottom, backDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, tmpDC);
}


void InitializeDC(HDC *dc, HBITMAP *bit)
{
	*dc = CreateCompatibleDC(screenDC);
	*bit = CreateCompatibleBitmap(screenDC, rect.right, rect.bottom);
	DeleteObject(SelectObject(*dc, *bit));
}

void InitializeAllDCs(HWND hWnd)
{
	screenDC = GetDC(hWnd);

	AddWndMenu(hWnd);

	brush = HBRUSH(GetStockObject(NULL_BRUSH));
	pen = HPEN(GetStockObject(BLACK_PEN));


	GetClientRect(hWnd, &rect);

	InitializeDC(&backDC, &backBit);
	FillWhiteRectangle(backDC, rect);

	InitializeDC(&frontDC, &frontBit);
	FillWhiteRectangle(frontDC, rect);

	SelectHandleForAllDCs(pen);
	SelectHandleForAllDCs(brush);

	SelectObject(screenDC, GetStockObject(SYSTEM_FIXED_FONT));

	ReleaseDC(hWnd, screenDC);
}


UINT ExecuteCommand(HWND hWnd, WPARAM wParam, LPARAM lParam, UINT message)
{
	wmId = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// Разобрать выбор в меню:

	if (IsInstrumentId(wmId))
	{
		DeactivateScaleAndSaveInstrumentId(hWnd, wmId);
		return 0;
	}

	switch (wmId)
	{
	case ID_BUTTONABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;

	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;

	case ID_BUTTONZOOM:
		checkedInstrumentId = ID_BUTTONZOOM;
		InvalidateRect(hWnd, nullptr, FALSE);
		break;

	case ID_BUTTONWIDTH1:
		ChangePen(1);
		break;

	case ID_BUTTONWIDTH2:
		ChangePen(2);
		break;

	case ID_BUTTONWIDTH3:
		ChangePen(3);
		break;

	case ID_BUTTONWIDTH4:
		ChangePen(4);
		break;

	case ID_BUTTONWIDTH5:
		ChangePen(5);
		break;

	case ID_BUTTONWIDTH6:
		ChangePen(6);
		break;

	case ID_BUTTONPRINT:
		// Заполняем PRINTDLG
		ZeroMemory(&printDialog, sizeof(printDialog));
		printDialog.lStructSize = sizeof(printDialog);
		printDialog.hwndOwner = hWnd;
		printDialog.hDevMode = nullptr; // Не забудьте освободить или сохранить hDevMode
		printDialog.hDevNames = nullptr; // Не забудьте освободить или сохранить hDevNames
		printDialog.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
		printDialog.nCopies = 1;
		printDialog.nFromPage = 0xFFFF;
		printDialog.nToPage = 0xFFFF;
		printDialog.nMinPage = 1;
		printDialog.nMaxPage = 0xFFFF;


		if (PrintDlg(&printDialog))
		{
			printDocInfo.cbSize = sizeof(DOCINFO);
			printDocInfo.lpszDocName = L"Print Picture";
			printDocInfo.fwType = NULL;
			printDocInfo.lpszDatatype = nullptr;
			printDocInfo.lpszOutput = nullptr;

			StartDoc(printDialog.hDC, &printDocInfo);

			StartPage(printDialog.hDC);

			screenDC = GetDC(hWnd);
			GetClientRect(hWnd, &rect);
			tmpDC = CreateCompatibleDC(screenDC);
			tmpBit = CreateCompatibleBitmap(screenDC, rect.right, rect.bottom);
			tmpHandle = SelectObject(tmpDC, tmpBit);


			int Rx = GetDeviceCaps(printDialog.hDC, LOGPIXELSX);
			int Ry = GetDeviceCaps(printDialog.hDC, LOGPIXELSY);

			int Rx1 = GetDeviceCaps(tmpDC, LOGPIXELSX);
			int Ry1 = GetDeviceCaps(tmpDC, LOGPIXELSY);

			int Rx2 = GetDeviceCaps(screenDC, LOGPIXELSX);
			int Ry2 = GetDeviceCaps(screenDC, LOGPIXELSY);

			double koef = min(double(Rx) / Rx1, double(Ry) / Ry1);


			FillRect(tmpDC, &rect, WHITE_BRUSH);
			StretchBlt(tmpDC, 0, 0, int(rect.right*scale), int(rect.bottom*scale),
				backDC, xBegin, yBegin, rect.right, rect.bottom, SRCCOPY);

			StretchBlt(printDialog.hDC, 0, 0,
				int(rect.right*koef*0.55),
				int(rect.bottom*koef*0.55),
				tmpDC, 0, 0, rect.right, rect.bottom, SRCCOPY);


			SelectObject(tmpDC, tmpHandle);
			DeleteObject(tmpBit);
			DeleteDC(tmpDC);

			EndPage(printDialog.hDC);

			EndDoc(printDialog.hDC);

			DeleteDC(printDialog.hDC);
			ReleaseDC(hWnd, screenDC);
		}
		/*
		*/
		break;

	case CM_FILE_OPEN:
		hMetaFileDlgBox.lStructSize = sizeof(OPENFILENAME);
		hMetaFileDlgBox.hwndOwner = hWnd;
		hMetaFileDlgBox.hInstance = hInst; // дескриптор копии приложения
		hMetaFileDlgBox.lpstrFilter = L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
		hMetaFileDlgBox.nFilterIndex = 1;
		hMetaFileDlgBox.lpstrFile = fullpath;
		hMetaFileDlgBox.nMaxFile = sizeof(fullpath);
		hMetaFileDlgBox.lpstrFileTitle = filename;
		hMetaFileDlgBox.nMaxFileTitle = sizeof(filename);
		hMetaFileDlgBox.lpstrInitialDir = nullptr;
		hMetaFileDlgBox.lpstrTitle = L"Open file...";
		hMetaFileDlgBox.Flags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_ALLOWMULTISELECT;

		if (GetOpenFileName(&hMetaFileDlgBox))
		{
			scale = 1;
			xBegin = 0;
			yBegin = 0;

			HENHMETAFILE hMetaFile = GetEnhMetaFile(fullpath);
			PlayEnhMetaFile(backDC, hMetaFile, &rect);
			//BitBlt(backDC, 0, 0, rect.right, rect.bottom, fileDC, 0, 0, SRCCOPY);
			FromBackDCToFrontDC(hWnd);

			DeleteEnhMetaFile(hMetaFile);

			InvalidateRect(hWnd, nullptr, FALSE);
		}
		break;
	
	case ID_BUTTONNEW:
		InitializeAllDCs(hWnd);
		break;

	case CM_FILE_SAVE:
		hMetaFileDlgBox.lStructSize = sizeof(OPENFILENAME);
		hMetaFileDlgBox.hwndOwner = hWnd;
		hMetaFileDlgBox.hInstance = hInst; // дескриптор копии приложения
		hMetaFileDlgBox.lpstrFilter = L"Metafile (*.emf)\0*.emf\0Все файлы (*.*)\0*.*\0";
		hMetaFileDlgBox.nFilterIndex = 1;
		hMetaFileDlgBox.lpstrFile = fullpath;
		hMetaFileDlgBox.nMaxFile = sizeof(fullpath);
		hMetaFileDlgBox.lpstrFileTitle = filename;
		hMetaFileDlgBox.nMaxFileTitle = sizeof(filename);
		hMetaFileDlgBox.lpstrInitialDir = nullptr;
		hMetaFileDlgBox.lpstrTitle = L"Save file as...";
		hMetaFileDlgBox.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
		
		if (GetSaveFileName(&hMetaFileDlgBox))
		{
			HDC fileDC = CreateEnhMetaFile(backDC, fullpath, nullptr, L"Save file");
			BitBlt(fileDC, 0, 0, rect.right, rect.bottom, backDC, 0, 0, SRCCOPY);
			DeleteEnhMetaFile(CloseEnhMetaFile(fileDC));
		}
		break;

	case ID_BUTTONCOLORPEN:
		penColorChooser.lStructSize = sizeof(CHOOSECOLOR);
		penColorChooser.hInstance = NULL;
		penColorChooser.hwndOwner = hWnd;
		penColorChooser.lpCustColors = crCustColor;
		penColorChooser.Flags = CC_RGBINIT | CC_FULLOPEN;
		penColorChooser.lCustData = 0L;
		penColorChooser.lpfnHook = NULL;
		penColorChooser.rgbResult = RGB(0x80, 0x80, 0x80);
		penColorChooser.lpTemplateName = NULL;

		if (ChooseColor(&penColorChooser))
		{
			ChangePen(penWidth);
		}

		break;
	case ID_BUTTONCOLORFILL:
		brushColorChooser.lStructSize = sizeof(CHOOSECOLOR);
		brushColorChooser.hInstance = NULL;
		brushColorChooser.hwndOwner = hWnd;
		brushColorChooser.lpCustColors = crCustColor;
		brushColorChooser.Flags = CC_RGBINIT | CC_FULLOPEN;
		brushColorChooser.lCustData = 0L;
		brushColorChooser.lpfnHook = NULL;
		brushColorChooser.rgbResult = RGB(0x80, 0x80, 0x80);
		brushColorChooser.lpTemplateName = NULL;

		if (ChooseColor(&brushColorChooser))
		{
			DeleteObject(brush);
			brush = CreateSolidBrush(brushColorChooser.rgbResult);
			SelectHandleForAllDCs(brush);
		}
		break;
	case ID_BUTTONCOLORNONE:
		DeleteObject(brush);
		brush = HBRUSH(GetStockObject(NULL_BRUSH));
		SelectHandleForAllDCs(brush);
		break;
	default:
		checkedInstrumentId = 0;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
	case WM_CREATE:
		ShowWindow(hWnd, SW_MAXIMIZE);
		InitializeAllDCs(hWnd);
		break;

	case WM_COMMAND:
		return ExecuteCommand(hWnd, wParam, lParam, message);

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		isLeftMouseDown = true;

		switch (checkedInstrumentId)
		{
		case ID_BUTTONELLIPSE:
		case ID_BUTTONLINE:
		case ID_BUTTONPENCIL:
		case ID_BUTTTONRECTANGLE:
			xCoordShapeStart = xCoordShapeEnd = short(LOWORD(lParam));
			yCoordShapeStart = yCoordShapeEnd = short(HIWORD(lParam));
			break;
		case ID_BUTTONPOLYLINE:
		case ID_BUTTONPOLYGON:
			if (isPolyDrawing == false)
			{
				coordXPolyStart = xCoordShapeStart = xCoordShapeEnd = short(LOWORD(lParam));
				coordYPolyStart = yCoordShapeStart = yCoordShapeEnd = short(HIWORD(lParam));
				isPolyDrawing = true;
			}
			else
			{
				xCoordShapeEnd = short(LOWORD(lParam));
				yCoordShapeEnd = short(HIWORD(lParam));
				
				MoveToEx(frontDC, xCoordShapeStart, yCoordShapeStart, nullptr);
				LineTo(frontDC, xCoordShapeEnd, yCoordShapeEnd);

				InvalidateRect(hWnd, &rect, false);
				UpdateWindow(hWnd);
			}
			break;
		case ID_BUTTONTEXT:
			isTextPrinting = true;
			break;
		case ID_BUTTONZOOM:
			xCoordShapeStart = xCoordShapeEnd = short(double(LOWORD(lParam)) / scale);
			yCoordShapeStart = yCoordShapeEnd = short(double(HIWORD(lParam)) / scale);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		if (!isLeftMouseDown || checkedInstrumentId == ID_BUTTONTEXT)
			break;

		xCoordShapeEnd = short(LOWORD(lParam));
		yCoordShapeEnd = short(HIWORD(lParam));

		if (checkedInstrumentId == ID_BUTTONPENCIL)
		{
			MoveToEx(backDC, xCoordShapeStart, yCoordShapeStart, nullptr);
			LineTo(backDC, xCoordShapeEnd, yCoordShapeEnd);
			xCoordShapeStart = xCoordShapeEnd;
			yCoordShapeStart = yCoordShapeEnd;
		}

		FromBackDCToFrontDC(hWnd);

		switch (checkedInstrumentId)
		{
		case ID_BUTTONLINE:
		case ID_BUTTONPOLYLINE:
		case ID_BUTTONPOLYGON:
			MoveToEx(frontDC, xCoordShapeStart, yCoordShapeStart, nullptr);
			LineTo(frontDC, xCoordShapeEnd, yCoordShapeEnd);
			break;
		case ID_BUTTTONRECTANGLE:
			Rectangle(frontDC, xCoordShapeStart
				, yCoordShapeStart, xCoordShapeEnd, yCoordShapeEnd);
				break;
		case ID_BUTTONELLIPSE:
			Ellipse(frontDC, xCoordShapeStart
				, yCoordShapeStart, xCoordShapeEnd, yCoordShapeEnd);
			break;
		case ID_BUTTONZOOM:
			xCoordShapeEnd = (short)((double)xCoordShapeEnd / scale);
			yCoordShapeEnd = (short)((double)yCoordShapeEnd / scale);
			xBegin += (xCoordShapeEnd - xCoordShapeStart);
			yBegin += (yCoordShapeEnd - yCoordShapeStart);
			xCoordShapeStart = xCoordShapeEnd;
			yCoordShapeStart = yCoordShapeEnd;
			break;
		}

		InvalidateRect(hWnd, &rect, false);
		UpdateWindow(hWnd);
		break;


	case WM_LBUTTONUP:
		ReleaseCapture();
		isLeftMouseDown = false;

		xCoordShapeEnd = short(LOWORD(lParam));
		yCoordShapeEnd = short(HIWORD(lParam));

		switch (checkedInstrumentId)
		{
		case ID_BUTTONLINE:
		case ID_BUTTONPOLYLINE:
		case ID_BUTTONPOLYGON:
			MoveToEx(backDC, xCoordShapeStart, yCoordShapeStart, nullptr);
			LineTo(backDC, xCoordShapeEnd, yCoordShapeEnd);
			break;
		case ID_BUTTTONRECTANGLE:
			Rectangle(backDC, xCoordShapeStart
				, yCoordShapeStart, xCoordShapeEnd, yCoordShapeEnd);
			break;
		case ID_BUTTONELLIPSE:
			Ellipse(backDC, xCoordShapeStart
				, yCoordShapeStart, xCoordShapeEnd, yCoordShapeEnd);
		case ID_BUTTONTEXT:
			text.clear();
			xTextCoord = short(LOWORD(lParam));
			yTextCoord = short(HIWORD(lParam));
			break;
		}


		if (checkedInstrumentId == ID_BUTTONPOLYLINE || checkedInstrumentId == ID_BUTTONPOLYGON)
		{
			xCoordShapeStart = xCoordShapeEnd;
			yCoordShapeStart = yCoordShapeEnd;
		}

		FromBackDCToFrontDC(hWnd);

		InvalidateRect(hWnd, &rect, false);
		UpdateWindow(hWnd);
		break;

	case WM_RBUTTONUP:
		xCoordShapeEnd = short(LOWORD(lParam));
		yCoordShapeEnd = short(HIWORD(lParam));

		if (isPolyDrawing)
		{
			MoveToEx(backDC, xCoordShapeStart, yCoordShapeStart, nullptr);
			LineTo(backDC, xCoordShapeEnd, yCoordShapeEnd);
			
			if (checkedInstrumentId == ID_BUTTONPOLYGON)
			{
				MoveToEx(backDC, coordXPolyStart, coordYPolyStart, nullptr);
				LineTo(backDC, xCoordShapeEnd, yCoordShapeEnd);
			}

		}

		isPolyDrawing  = false;
		isTextPrinting = false;

		FromBackDCToFrontDC(hWnd); 
		InvalidateRect(hWnd, &rect, false);
		UpdateWindow(hWnd);
		break;

	case  WM_CHAR:
		if (isTextPrinting)
		{
			WCHAR c = WCHAR(wParam);
			
			text += c;
			TextOut(backDC, xTextCoord, yTextCoord, text.c_str(), text.length());
			TextOut(frontDC, xTextCoord, yTextCoord, text.c_str(), text.length());
			InvalidateRect(hWnd, nullptr, FALSE);
			UpdateWindow(hWnd);
		}
		break;

	case WM_MOUSEWHEEL:
		isTextPrinting = false;
		if (checkedInstrumentId != ID_BUTTONZOOM)
		{
			scale = 1;
			checkedInstrumentId = ID_BUTTONZOOM;
		}
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if ((zDelta>0) && (scale<3))
			scale = scale + 0.03;
		if ((zDelta<0) && (scale>0.3))
			scale = scale - 0.03;
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);
		break;

	case WM_PAINT:
		PAINTSTRUCT p;

		screenDC = BeginPaint(hWnd, &p);

		if (checkedInstrumentId == ID_BUTTONZOOM)
		{
			GetClientRect(hWnd, &rect);
			tmpDC = CreateCompatibleDC(screenDC);
			tmpBit = CreateCompatibleBitmap(screenDC, rect.right, rect.bottom);
			tmpHandle = SelectObject(tmpDC, tmpBit);

			FillRect(tmpDC, &rect, WHITE_BRUSH);

			StretchBlt(tmpDC, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale),
				backDC, xBegin, yBegin, rect.right, rect.bottom, SRCCOPY);

			SelectObject(tmpDC, (HBRUSH)GetStockObject(NULL_BRUSH));
			SelectObject(tmpDC, (HPEN)GetStockObject(BLACK_PEN));
			Rectangle(tmpDC, 0, 0, (int)(rect.right*scale), (int)(rect.bottom*scale));

			BitBlt(screenDC, 0, 0, rect.right, rect.bottom, tmpDC, 0, 0, SRCCOPY);

			SelectObject(tmpDC, tmpHandle);
			DeleteObject(tmpBit);
			DeleteDC(tmpDC);
			break;
		}

		BitBlt(screenDC, 0, 0, rect.right, rect.bottom, frontDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &p);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return INT_PTR(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return INT_PTR(TRUE);
		}
		break;
	}
	return INT_PTR(FALSE);
}
