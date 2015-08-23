// zx.cpp : Defines the entry point for the application.
//

//options
#define DBL_BUFFER		//if uncomment, use dbl buffer


#define _save_main_debug_

#if defined(_save_main_debug_)
#define DebugMsg(txt) fprintf(main_debug_file,txt); fprintf(main_debug_file,"\n");
#else 
#define DebugMsg(txt)
#endif

/////////////////////
#ifdef DBL_BUFFER
#pragma message ("----------------------------------")
#pragma message ("--- Using Double Screen Buffer ---")
#pragma message ("----------------------------------")
#endif


#include "stdafx.h"
#include "zx.h"
#include <commctrl.h>
#include <aygshell.h>
#include <sipapi.h>
#include <gx.h>
#include <stdio.h>
#include "ZxEmulSrc\z80.h"
#include "ZxEmulSrc\zxfiles.h"
#include "ZxEmulSrc\wave.h"
#include "ZxEmulSrc\ay.h"
#include "ZxEmulSrc\TrDos.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;				// The current instance
//HWND				g_hwndCB;					// The command bar handle
GXDisplayProperties	g_DisplayProperties;

#ifdef DBL_BUFFER
LPVOID				g_hMem;
#endif

wave				snd;
ay					chip;
CTrDos				trdos;

static SHACTIVATEINFO s_sai;

	FILE* main_debug_file;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
//HWND				CreateRpCommandBar(HWND);
//void			ClearDisplay(void *MEM);
void			OutScr2Display(void *MEM, LPVOID hScrMem, bool bFlash);
void			FastOutScr2Display(void *MEM, LPVOID hScrMem, bool bFlash, zxbyte Border);
void			SetVKey(zxbyte x, zxbyte y, bool state);

WORD*			LoadSprite(LPCTSTR lpbname, SIZE& size);
void			OutSprite(void *MEM, WORD* pspr, SIZE& sprsize, int x, int y);
SIZE			gKeyboardSize;
WORD*			KeyboardSprite;
bool			ShowKeyboard;
zxbyte			LinesInKeyboard; //Выдвиннотусть клавы

zxbyte VirtualKeyboardInfo[][6]={
	//x,y,sx,sy,row,num
	1,1,13,12,0,0, /*Tab: CS*/	1,1,13,12,3,0, /*Tab: 1*/
	15,1,13,12,2,0, /*Q*/ 29,1,13,12,2,1, /*W*/ 43,1,13,12,2,2, /*E*/ 57,1,13,12,2,3, /*R*/ 71,01,13,12,2,4, /*T*/
	85,1,13,12,5,4,	/*Y*/ 99,1,13,12,5,3, /*U*/ 113,1,13,12,5,2,/*I*/ 127,1,13,12,5,1,/*O*/ 141,1,13,12,5,0, /*P*/
	155,1,13,13,6,0, /*Enter*/ 141,14,27,13,6,0, /*Enter*/
	169,1,13,12,0,0, /*Del: CS*/ 169,1,13,12,4,0, /*Del: 0*/
	1,14,13,13,0,0, /*CapsLock: CS*/ 1,14,13,13,3,1, /*CapsLock: 2*/
	15,14,13,13,1,0, /*A*/ 29,14,13,13,1,1, /*S*/ 43,14,13,13,1,2, /*D*/ 57,14,13,13,1,3, /*F*/ 71,14,13,13,1,4, /*G*/
	85,14,13,13,6,4,	/*H*/ 99,14,13,13,6,3, /*J*/ 113,14,13,13,6,2,/*K*/ 127,14,13,13,6,1,/*L*/
	1,28,13,13,0,0, /*Ext: CS*/ 1,28,13,13,7,1, /*Ext: SS*/
	15,28,13,13,0,0, /*CS*/ 29,28,13,13,0,1, /*Z*/ 43,28,13,13,0,2, /*X*/ 57,28,13,13,0,3, /*C*/ 71,28,13,13,0,4, /*V*/
	85,28,13,13,7,4,	/*B*/ 99,28,13,13,7,3, /*N*/ 113,28,13,13,7,2,/*M*/ 127,28,27,13,7,0,/*SPACE*/ 155,28,13,13,7,1, /*SS*/
	186,1,13,12,3,0, /*1*/	200,1,13,12,3,1, /*2*/	214,1,13,12,3,2, /*3*/	228,1,13,12,3,3, /*4*/	242,1,13,12,3,4, /*5*/
	186,14,13,13,4,4, /*6*/	200,14,13,13,4,3, /*7*/	214,14,13,13,4,2, /*8*/	228,14,13,13,4,1, /*9*/	242,14,13,13,4,0, /*0*/
	200,28,13,13,0,0, /*Graph: CS*/	200,28,13,13,4,1, /*Graph: 9*/
	214,28,13,13,0,0, /*TV: CS*/	214,28,13,13,3,3, /*TV: 4*/
	228,28,13,13,0,0, /*IV: CS*/	228,28,13,13,3,2, /*IV: 3*/

//Graph
//TruVideo
//InvVideo
0,0,0,0,0,0
};

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

#if defined(_save_main_debug_)	
	main_debug_file=fopen("maindbg.txt","w");
#endif

	DebugMsg("Debug file created.");

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ZX);
	ConnectTrDos(&trdos);

	if (lpCmdLine[0]==0) //LoadTRD(TEXT("stpro.trd"), trdos.CreateImage(0));
		LoadSNA(TEXT("dizzy4.sna"));
	else LoadSNA(lpCmdLine);

	ResetKeyboardZX();

	DebugMsg("Syntez1");
	chip.Syntez(snd);
	DebugMsg("Syntez2");
	chip.Syntez(snd);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	GXCloseDisplay();

#ifdef DBL_BUFFER
	delete[] g_hMem;
#endif

	delete[] KeyboardSprite;

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
    wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZX));
    wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}

//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND	hWnd = NULL;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	g_hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_ZX, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//If it is already running, then focus on the window
	hWnd = FindWindow(szWindowClass, szTitle);	
	if (hWnd) 
	{
		// set focus to foremost child window
		// The "| 0x01" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
		return 0;
	} 

	MyRegisterClass(hInstance, szWindowClass);
	
	hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{	
		return FALSE;
	}
	//When the main window is created using CW_USEDEFAULT the height of the menubar (if one
	// is created is not taken into account). So we resize the window after creating it
	// if a menubar is present
/*	if (g_hwndCB)
    {
		RECT rc;
        RECT rcMenuBar;

		GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hwndCB, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
		MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	} */

	RECT rc;
	GetWindowRect(hWnd, &rc);
	SHFullScreen(hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON |  SHFS_HIDESTARTICON);
	//MoveWindow( hWnd, rc.left, rc.top - GetSystemMetrics(SM_CYMENU), 
    //    rc.right, rc.bottom + GetSystemMetrics(SM_CYMENU), TRUE);
	MoveWindow( hWnd, 0, 0, 
        rc.right, rc.bottom + GetSystemMetrics(SM_CYMENU), TRUE);

	GXOpenDisplay(hWnd,GX_FULLSCREEN);
	g_DisplayProperties=GXGetDisplayProperties ();

#ifdef DBL_BUFFER
	g_hMem=(LPVOID) new WORD[320*240+1];
#endif
	LinesInKeyboard=0;
	ShowKeyboard=false; //true;
	KeyboardSprite=LoadSprite(MAKEINTRESOURCE(IDB_KEYBOARD), gKeyboardSize);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	snd.init(hWnd);

	return TRUE;
}



//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	static UINT uTimer;
	static int flgborder=2;
	static bool flg=true;
	static bool	flgpause=false;
	//virtual keyboard
	static int VkeyX=0;
	static int VkeyY=0;
	static bool Vkeypressed=false;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{	
//				case IDM_HELP_ABOUT:
//					DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
//				    break;
				case IDOK:
					SendMessage (hWnd, WM_CLOSE, 0, 0);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
            // Initialize the shell activate info structure
            memset (&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
			uTimer=SetTimer(hWnd,1,1000/4,NULL);
			uTimer=SetTimer(hWnd,2,1000/50,NULL);

			GXOpenInput();


			break;
		case WM_PAINT:
			DebugMsg("WM_PAINT");

//			int *drawmem;
			void *drwme;

			hdc = BeginPaint(hWnd, &ps);
			drwme=GXBeginDraw ();
			//drawmem=(int*)drwme;
//			if (flgborder!=0) {
//				ClearDisplay(drwme);
//				flgborder--;
//			};
			//OutScr2Display(drwme,GetMemPtr(16384),flg);

			FastOutScr2Display(drwme,GetScrPtr(),flg,GetBorderColor());

			GXEndDraw ();
			EndPaint(hWnd, &ps);
			break;
		case WM_TIMER:
			DebugMsg("WM_TIMER1");
			if(wParam==1) {						//Таймер флэша
			flg=!flg;
			//InvalidateRect(hWnd,NULL,false);	//На случай вылетания звуковой системы.
			//UpdateWindow(hWnd);					//Хоть и с тормозами. Только ведь без процессора то нехрен тут делать обновлению экрана.
//				if (flgpause) break;
//				while (!step());
//				InvalidateRect(hWnd,NULL,false);
//				UpdateWindow(hWnd);
			};
			if(wParam==2) {						//Таймер для режима без звука (и глюков)
			  while (!step());
			  InvalidateRect(hWnd,NULL,false);	//На случай вылетания звуковой системы.
			  UpdateWindow(hWnd);					//Хоть и с тормозами. Только ведь без процессора то нехрен тут делать обновлению экрана.
//				if (flgpause) break;
//				while (!step());
//				InvalidateRect(hWnd,NULL,false);
//				UpdateWindow(hWnd);
			};

			break;
		case WOM_DONE:
			DebugMsg("WOM_DONE");
			while (!step());
			//InvalidateRect(hWnd,NULL,false);
			//UpdateWindow(hWnd);
			chip.CopyRegs(GetAYRegs());
			//chip.Syntez(snd);
			break;
		case WM_DESTROY:
			KillTimer(hWnd,uTimer);
			GXCloseInput();

#if defined(_save_main_debug_)
	fclose(main_debug_file);
#endif

//			CommandBar_Destroy(g_hwndCB);
			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
            // Notify shell of our activate message
			SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
     		break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
     		break;
		case WM_LBUTTONDOWN: //нажали стилусом
			DebugMsg("WM_LBUTTON");
			if (ShowKeyboard) {
				if (!Vkeypressed) {
					VkeyX = HIWORD(lParam)-(320-gKeyboardSize.cx)/2;
					VkeyY = LinesInKeyboard-LOWORD(lParam);
					if (VkeyX<0 || VkeyX>=gKeyboardSize.cx || VkeyY<0 || VkeyY>=gKeyboardSize.cy) {
						VkeyX=0; VkeyY=0;
					} else {
						SetVKey(VkeyX, VkeyY, true);
						Vkeypressed=true;
					};
				};
			} else {
				SendMessage (hWnd, WM_CLOSE, 0, 0);
			};
			break;
		case WM_LBUTTONUP:	//отпустили стилус
			if (Vkeypressed) {
				SetVKey(VkeyX, VkeyY, false);
				Vkeypressed=false;
			};
			break;
		case WM_KEYDOWN:
			DebugMsg("WM_KEYDOWN");
			switch (wParam) {
			case 0x28: //hard key down (right при повороте экрана)
				SetKeyZX(4,3,true); //set 6 left
				return 0;
			case 0x26: //hard key up
				SetKeyZX(4,4,true); //set 7 right
				return 0;
			case 0x25: //hard key left
				SetKeyZX(4,2,true); //set 8 down
				return 0;
			case 0x27: //hard key right
				SetKeyZX(4,1,true); //set 9 up
				return 0;
			//case 0x0d: //hard key fire/enter
			//	SetKeyZX(6,0,true); //set Enter
			//	return 0;
			case 0xc1:
				SetKeyZX(4,0,true); //set 0 fire
				return 0;
			};
			break;
		case WM_KEYUP:
			switch (wParam) {
			case 0x28: //hard key down (right при повороте экрана)
				SetKeyZX(4,3,false); //set 6 left
				return 0;
			case 0x26: //hard key up
				SetKeyZX(4,4,false); //set 7 right
				return 0;
			case 0x25: //hard key left
				SetKeyZX(4,2,false); //set 8 down
				return 0;
			case 0x27: //hard key right
				SetKeyZX(4,1,false); //set 9 up
				return 0;
			//case 0x0d: //hard key fire/enter
			//	SetKeyZX(6,0,false); //set Enter
			//	return 0;
			case 0xc1:
				SetKeyZX(4,0,false); //set 0 fire
				return 0;
			case 0x0d://0xc5:
				ShowKeyboard=!ShowKeyboard;
				return 0;
			};

			break;
		case WM_KILLFOCUS:
			flgpause=true;
			GXSuspend (); //todo: проверить результат!!!
			break;
		case WM_SETFOCUS:
			flgborder=true;
			flgpause=false;

			GXResume (); //todo: проверить результат!!!
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

/*
void ClearDisplay(void *MEM) {
	__int16 clcol=0;
	//clcol+=10;
	__int16* dMEM=(__int16*)MEM;
	for (long lo=0;	lo<(long)(g_DisplayProperties.cxWidth*g_DisplayProperties.cyHeight*g_DisplayProperties.cBPP/8/sizeof(clcol));lo++) {
		*dMEM++=clcol;
	};
	//clcol+=10;
};

//
// Speccy screen reference...
//
//	Если x=0..255 и y=0..191 то:
//	x/8=0..31		координата x знакоместа				*1
//	x%8=0..7		координата x в знакоместе			для маски использовать
//	y/64=0..3		номер трети экрана по y				*2048
//	y%64%8=0..7		координата y в знакоместе			*256
//	y%64/8=0..7		координата y знакоместа в трети      *32

void OutScr2Display(void *MEM, LPVOID hScrMem, bool bFlash) {
	__int16 clcol;
	unsigned __int8 bright;
//	unsigned __int8 r;
//	unsigned __int8 g;
//	unsigned __int8 b;
	bool mbyte;
	unsigned __int8 abyte;
	unsigned __int8* hSM=(unsigned __int8*)hScrMem;

	__int16* dMEM=(__int16*)MEM;
 		for (unsigned short x=0;x<256;x++) 	for (unsigned short y=0;y<192;y++){
			mbyte=(*(hSM+x/8+y/64*2048+y%64%8*256+y%64/8*32)) >>(7-x%8)&1;
			abyte=*(hSM+6144+x/8+y/8*32);
			if (abyte & 128) mbyte^=bFlash;
			if (abyte & 64) bright=255;
			else bright=192;
			if (!mbyte) abyte>>=3;
			abyte&=7;
			switch (g_DisplayProperties.cBPP) {
			case 16:
				switch (g_DisplayProperties.ffFormat) {
				case kfDirect555|kfDirect:
					clcol=((bright>>3)&31)*((abyte&1)+((abyte&2)<<9)+((abyte&4)<<3)); // проверить!!!
					break;
				case kfDirect565|kfDirect:
					//                         blue      red                                 green
					clcol=((bright>>3)&31)*((abyte&1)+((abyte&2)<<10)) + ((bright>>2)&63)*(((abyte&4)<<3));
					break;
				default:
					break;
				};
				break;
			default:
				break;
			};
			//clcol=127*(mbyte);

			*(dMEM+(((g_DisplayProperties.cyHeight-256)/2+x)*g_DisplayProperties.cbyPitch/2+
					(191-y+(g_DisplayProperties.cxWidth-192)/2))*g_DisplayProperties.cbxPitch/2)=clcol;
		};
};
*/
// Функцию CalcColor надо будет разместить в инициализации программы,
// а вызов заменить на обращение к массиву
//
__int16 CalcColor(int color) {
	int bright=((color<8)?192:255);
		switch (g_DisplayProperties.cBPP) {
		case 16:
			if ((g_DisplayProperties.ffFormat&kfDirect555)!=0) 
				return ((bright>>3)&31)*((color&1)+((color&2)<<9)+((color&4)<<3)); // проверить!!!
			if ((g_DisplayProperties.ffFormat&kfDirect565)!=0)
				//                         blue      red                                 green
				return ((bright>>3)&31)*((color&1)+((color&2)<<10)) + ((bright>>2)&63)*(((color&4)<<3));
		//	default:
		//		break;
		//	};
			return 0;
			break;
		default:
			break;
		};
		return 0;
/*
    #define kfLandscape 0x8         // Screen is rotated 270 degrees
    #define kfPalette   0x10        // Pixel values are indexes into a palette
    #define kfDirect    0x20        // Pixel values contain actual level information
    #define kfDirect555 0x40        // 5 bits each for red, green and blue values in a pixel.
    #define kfDirect565 0x80        // 5 red bits, 6 green bits and 5 blue bits per pixel
    #define kfDirect888 0x100       // 8 bits each for red, green and blue values in a pixel.
    #define kfDirect444 0x200       // 4 red, 4 green, 4 blue
    #define kfDirectInverted 0x400
*/
};

struct BGRColor {
	BYTE colorB;
	BYTE colorG;
	BYTE colorR;
};

__int16 CalcColorBGR(BGRColor clr) {
	switch (g_DisplayProperties.cBPP) {
	case 16:
		if ((g_DisplayProperties.ffFormat&kfDirect555)!=0)
			return ((clr.colorR>>3)<<10) + ((clr.colorG>>3)<<5) + (clr.colorB>>3); // проверить!!!
		if ((g_DisplayProperties.ffFormat&kfDirect565)!=0)
			return ((clr.colorR>>3)<<11) + ((clr.colorG>>2)<<5) + (clr.colorB>>3);
		return 0;
	default:
		return 0;
	};
};

void FastOutScr2Display(void *MEM, LPVOID hScrMem, bool bFlash, zxbyte Border) {
	static zxbyte OldBorder=8;
	__int16 BorderColor;
	__int16 InkColor;
	__int16 PaperColor;

	int i,j,k;

	unsigned __int8 mbyte;
	unsigned __int8 abyte;

	unsigned __int8* hGfx=(unsigned __int8*)hScrMem;
	unsigned __int8* hAtrs=hGfx+6144;
	DWORD sx=g_DisplayProperties.cxWidth;
	DWORD sy=g_DisplayProperties.cyHeight;
	long px=g_DisplayProperties.cbxPitch / 2;
	long py=g_DisplayProperties.cbyPitch / 2;

#ifdef DBL_BUFFER
	__int16* dMEM=(__int16*)g_hMem+(sx-1)*px;	//with dbl_buffer
#else
	__int16* dMEM=(__int16*)MEM+(sx-1)*px;		//no dbl_buffer
#endif

//for rotate screen:
//	__int16* dMEM=(__int16*)MEM+(sy-1)*py;
//	px=-px;
//	py=-py;

	if (ShowKeyboard) {
		if (LinesInKeyboard<gKeyboardSize.cy) {LinesInKeyboard+=4; OldBorder=8;};
	} else {
		if (LinesInKeyboard>0) {LinesInKeyboard-=4; OldBorder=8;};
	};

	int UpBorderLines=(48-LinesInKeyboard)/2;
	int DownBorderLines=48-(48-LinesInKeyboard)/2;

	BorderColor=CalcColor(Border);
	if (OldBorder==Border) {
		dMEM-=UpBorderLines*px;
	} else {
		for (j=UpBorderLines;j--!=0;) {
			for (i=sy; i--!=0; dMEM+=py) *dMEM=BorderColor;
			dMEM-=py*sy+px;
		};
	};
	for (k=0;k<3;k++) {
		for (j=0;j<64;j++) {
			for (i=32; i--!=0; dMEM+=py) *dMEM=BorderColor;
			for (i=0;i<32;i++) {
				mbyte=*(hGfx++);
				abyte=*(hAtrs++);
				InkColor=CalcColor((abyte&7)|((abyte&64)>>3));
				PaperColor=CalcColor((abyte>>3)&15);
				if ((bFlash)&&((abyte&128)!=0)) mbyte^=255;
				*(dMEM)=(mbyte&128)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&64)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&32)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&16)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&8)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&4)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&2)?InkColor:PaperColor;
				*(dMEM+=py)=(mbyte&1)?InkColor:PaperColor;
				dMEM+=py;
			};
			for (i=32; i--!=0; dMEM+=py) *dMEM=BorderColor;
			dMEM-=py*sy+px;
		if ((j&7)==7) hGfx-=256*7; else { hGfx+=256-32; hAtrs-=32; };
		};
	hGfx+=2048-32*8;
	};
	if (OldBorder==Border) {
		dMEM-=DownBorderLines*px;
	} else {
		for (j=DownBorderLines;j--!=0;) {
			for (i=sy; i--!=0; dMEM+=py) *dMEM=BorderColor;
			dMEM-=py*sy+px;
		};
	};
	
	OldBorder=Border;

	if (LinesInKeyboard!=0) {

		OutSprite(
#ifdef DBL_BUFFER
			g_hMem,
#else
			MEM,
#endif
			KeyboardSprite, gKeyboardSize, 240-LinesInKeyboard,(320-gKeyboardSize.cx)/2);
	};

#ifdef DBL_BUFFER
	memcpy(MEM, g_hMem, 320*240*2);
//!or!
//	DWORD* DWordSrc=(DWORD*) g_hMem;
//	DWORD* DWordDst=(DWORD*) MEM;
//	for (i=0;i<320*240*2/4;i++) {
//		*(DWordDst++)=*(DWordSrc++);
//	};
#endif

};

void SetVKey(zxbyte x, zxbyte y, bool state) {
//	zxbyte VirtualKeyboardInfo[][6]={
	int i=0;
	while (VirtualKeyboardInfo[i][0]!=0 && VirtualKeyboardInfo[i][1]!=0) {
		if (x>=VirtualKeyboardInfo[i][0])
			if (x<(VirtualKeyboardInfo[i][0]+VirtualKeyboardInfo[i][2]))
				if (y>=VirtualKeyboardInfo[i][1])
					if (y<(VirtualKeyboardInfo[i][1]+VirtualKeyboardInfo[i][3])) {
						SetKeyZX(VirtualKeyboardInfo[i][4],VirtualKeyboardInfo[i][5],state);
					};
	i++;
	};
};

void OutSprite(void *MEM, WORD* pspr, SIZE& sprsize, int x, int y) {
	int i,j;
	int ssx=sprsize.cx;
	int ssy=sprsize.cy;
	if ((ssx+y)>320) ssx=320-y;
	if ((ssy+x)>240) ssy=240-x;

	DWORD sx=g_DisplayProperties.cxWidth;
	DWORD sy=g_DisplayProperties.cyHeight;
	long px=g_DisplayProperties.cbxPitch / 2;
	long py=g_DisplayProperties.cbyPitch / 2;

	__int16* dMEM=(__int16*)MEM+(sx-1)*px;
	dMEM-=x*px;
	dMEM+=y*py;

	for (j=0;j<ssy;j++) {
		__int16* dm=dMEM-j*px;
		__int16* sm=(__int16*)pspr+j*sprsize.cx;
		for (i=0;i<ssx;i++) {
			*dm=*sm++;
			dm+=py;
		};
	};
};

// SIZE keyboardspritesize;
// LoadSprite(MAKEINTRESOURCE(IDB_KEYBOARD), &keyboardspritesize);

WORD* LoadSprite(LPCTSTR lpbname, SIZE& size) {
	HBITMAP hSpr = LoadBitmap(g_hInst,lpbname);

	BITMAP bminfo;
	GetObject(hSpr,sizeof(BITMAP),&bminfo);

	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bminfo.bmWidth;
	bmi.bmiHeader.biHeight = bminfo.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 3780; //from stgapibuffer library
	bmi.bmiHeader.biYPelsPerMeter = 3780;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiColors[0].rgbBlue = 0;
	bmi.bmiColors[0].rgbGreen = 0;
	bmi.bmiColors[0].rgbRed = 0;
	bmi.bmiColors[0].rgbReserved = 0;
	
	HDC hDC=GetDC(NULL);
	void* pbuf;
	HBITMAP hTarget = CreateDIBSection(hDC,&bmi,DIB_RGB_COLORS,&pbuf,NULL,0);
	ReleaseDC(NULL,hDC);

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcDst = CreateCompatibleDC(NULL);

	HBITMAP hbmOld = (HBITMAP) SelectObject(hdcSrc, hSpr);
	HBITMAP hbmOld2 = (HBITMAP) SelectObject(hdcDst, hTarget);

	BitBlt(hdcDst, 0, 0, bminfo.bmWidth, bminfo.bmHeight, hdcSrc, 0, 0, SRCCOPY);

	SelectObject(hdcSrc, hbmOld);
	SelectObject(hdcDst, hbmOld2);
	DeleteDC(hdcSrc);
	DeleteDC(hdcDst);

	DeleteObject(hSpr);

	BGRColor* pbufbgr= (BGRColor*) pbuf;
	WORD* pbufrez=new WORD[bminfo.bmWidth*bminfo.bmHeight];
	
	WORD* pbufdst=pbufrez;
	for (int y=0;y<bminfo.bmHeight;y++) {
		BGRColor* pbufsrc=pbufbgr+((bminfo.bmWidth+3)/4)*4*(bminfo.bmHeight-y-1);
		for (int x=0;x<bminfo.bmWidth;x++) {
			*pbufdst=CalcColorBGR(*pbufsrc);
			pbufdst++;
			pbufsrc++;
		};
	};
	size.cx=bminfo.bmWidth;
	size.cy=bminfo.bmHeight;
	DeleteObject(hTarget);
	return pbufrez;
};
