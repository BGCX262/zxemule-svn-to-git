/*
 * uApplication.cpp
 * Унифицированное приложение. Содержит стандартизированные функции для запросов из uSystem
 *
 *  Created on: 08.11.2009
 *      Author: capry
 */

//Название неудачное... наверное. переобозвать из sal в "приложение", т.е. в application.

#include "uSystem.h"
#include "uApplication.h"
#include "ZxEmulSrc\z80.h"
#include "ZxEmulSrc\zxfiles.h"
//#include "ZxEmulSrc\wave.h"
//#include "ZxEmulSrc\ay.h"
#include "ZxEmulSrc\TrDos.h"
#include "uTypes.h"

//SAL instance;
CuSystem* uSys;

uTInt flash;

struct keyMap {
	uTInt uScan;
	zxbyte row;
	zxbyte nkey;
};

//	Бит  адреса		Сброшенный бит		  Бит  адреса
//			         	D0	D1	D2	D3	D4	D4	D3	D2	D1	D0		
//	11	п	#F7FE	A11	1	2	3	4	5	6	7	8	9	0	А12	#EFFE	п	12
//	10	о	#FBFE	A10	Q	W	E	R	Т	Y	U	I	О	Р	А13	#DFFE	о	13
//	9	Р	#FDFE	A9	A	S	D	F	G	H	J	К	L	EN	А14	#BFFE	Р	14
//	8	т	#FEFE	A8	CS	Z	X	С	V	В	N	М	SS	SP	А15	#7FFE	т	15


keyMap keyMaps[]={
	{uKeyLeftShift,  0,0}, //Caps Shift
	{uKeyRightShift, 0,0}, //Caps Shift
	
	{'Z',       0,1},
	{'X',       0,2},
	{'C',       0,3},
	{'V',       0,4},
	{'A',       1,0},
	{'S',       1,1},
	{'D',       1,2},
	{'F',       1,3},
	{'G',       1,4},
	{'Q',       2,0},
	{'W',       2,1},
	{'E',       2,2},
	{'R',       2,3},
	{'T',       2,4},
	{'1',       3,0}, //1
	{'2',       3,1}, //2
	{'3',       3,2}, //3
	{'4',       3,3}, //4
	{'5',       3,4}, //5
	{'0',       4,0}, //0
	{'9',       4,1}, //9
	{'8',       4,2}, //8
	{'7',       4,3}, //7
	{'6',       4,4}, //6

	//{uKeySelect,4,0},
	//{uKeyUp,    4,1},
	//{uKeyDown,  4,2},
	//{uKeyRight, 4,3},
	//{uKeyLeft,  4,4},

	//cursor joystick + caps
	{uKeySelect,4,0}, //{uKeySelect,0,0},
	{uKeyUp,    4,3}, {uKeyUp,    0,0},
	{uKeyDown,  4,4}, {uKeyDown,  0,0},
	{uKeyRight, 4,2}, {uKeyRight, 0,0},
	{uKeyLeft,  3,4}, {uKeyLeft,  0,0},
	
	{'P',       5,0},
	{'O',       5,1},
	{'I',       5,2},
	{'U',       5,3},
	{'Y',       5,4},
	{uKeyEnter, 6,0}, //Enter
	{'L',       6,1},
	{'K',       6,2},
	{'J',       6,3},
	{'H',       6,4},
	{uKeySpace, 7,0}, //SPACE
	{uKeyLeftCtrl,  7,1}, //Symbol Shift
	{uKeyRightCtrl, 7,1}, //Symbol Shift
	{'M',       7,2},
	{'N',       7,3},
	{'B',       7,4},
	{uKeyBackspace,4,0}, {uKeyBackspace,0,0},  //0+CS
	{uKeyComma,    7,1}, {uKeyComma,    7,3},  //SS+N для ,
	{uKeyFullStop, 7,1}, {uKeyFullStop, 7,2},  //SS+M для .

	{uEndOfKeys,0,0}
};

CTrDos trdos;

void FastOutScr2Display(void *MEM, uTUint32 hScrMem, bool bFlash, zxbyte Border, uTInt sx, uTInt sy, uTDisplayMode displayMode);

void uAppInit(CuSystem* salInst) {
	uSys=salInst;
	//ConnectTrDos(&trdos);
	//if (lpCmdLine[0]==0) //
	ConnectTrDos(&trdos);
	LoadTRD("c:\\data\\zxemule\\mix05.trd", trdos.CreateImage(0));
	//LoadSNA("c:\\data\\zxemule\\dizzy5.sna");
	//else LoadSNA(lpCmdLine);

}

void uAppTimers(void) {
	static uTInt flashCounter;
	if ((flashCounter++)>10) {
		flashCounter=0;
		if (flash==0) flash=1;
		else flash=0;
	};
	while (!step());
	//SetMemZX(0x4000,GetWordMemZX(0x4000)+1 );
}

uTBool uAppInput(uTInt Event, uTInt Scancode) {
	//Если нажата или отпущена клавиша - ищу маппинг, если нашел - делаю действия
	if (Event==uEventKeyDown || Event==uEventKeyUp) {
		uTInt i=0;
		uTInt k=0;
		while (keyMaps[i].uScan!=uEndOfKeys) {
			if (keyMaps[i].uScan==Scancode) {
				SetKeyZX((zxbyte)keyMaps[i].row,(zxbyte)keyMaps[i].nkey,Event==uEventKeyDown?true:false);
				k++;
			};
			i++;
		};
		if (k!=0) return true;
	};
	return false;
}

void uAppUpdateDisplay( uTUint32 addr ,uTInt Width, uTInt Height, uTDisplayMode displayMode) {

switch (displayMode) {
    case uEColor64K:
        // ЗАМЕЧАНИЕ: TUint16* применим лишь для 16bit изображений;
        uTUint16* bitmapData = (uTUint16*)addr;
        for ( uTInt y = 0; y < Height; y++ ) {
    		for ( uTInt x = 0; x < Width; x++ ) {
    			 //Увеличиваем красную составляющую рисунка
    			*bitmapData = ( *bitmapData & 31 ) | // blue
                ( ((( *bitmapData >> 5 ) & 63 )+1)<<5) | // green
                ( ((( (*bitmapData >> 11) ) & 31)+1)<<11 ); // red
    			bitmapData++;    			
            };
         };
    	break;
    case uEColor16MU:
//        uTUint32* bitmapData32 = (uTUint32*)addr;
//        for ( uTInt y = 0; y < Height; y++ ) {
//    		for ( uTInt x = 0; x < Width; x++ ) {
//    			 //Увеличиваем красную составляющую рисунка
//    			*bitmapData32 = //( *bitmapData32  ) | // blue
//                ( ((( *bitmapData32 >> 8 ) & 255) +1)<<8) | // green
//                ( ((( *bitmapData32 >> 16 ) & 255)+1)<<16 ); // red
//    			bitmapData32++;    			
//            }
//         };
        FastOutScr2Display((void*)addr, (unsigned long) GetScrPtr(), flash, GetBorderColor(), Width, Height, displayMode);
    	break;
    default:
    	break;
    };

}

void uAppSystemCallback(void) {

}

void uAppQuit(void) {

}

// Функцию CalcColor надо будет разместить в инициализации программы,
// а вызов заменить на обращение к массиву
//
uTUint32 CalcColor(int color, uTDisplayMode displayMode) {
	int bright=((color<8)?192:255);
		switch (displayMode) {
		case uEColor64K: //r5g6b5
			//if ((g_DisplayProperties.ffFormat&kfDirect555)!=0) 
			//	return ((bright>>3)&31)*((color&1)+((color&2)<<9)+((color&4)<<3)); // проверить!!!
			//if ((g_DisplayProperties.ffFormat&kfDirect565)!=0)
				//                         blue      red                                 green
				//return ((bright>>3)&31)*((color&1)+((color&2)<<10)) + ((bright>>2)&63)*(((color&4)<<3));
			return ((bright>>3)&31)*((color&1)+((color&2)<<10)) + ((bright>>2)&63)*(((color&4)<<3));
		case uEColor16MU: //r8g8b8
			return bright*((color&1)+((color&2)<<15) + ((color&4)<<6));
			break;
		default:
			break;
		};
		return 0;
};

void FastOutScr2Display(void *MEM, uTUint32 hScrMem, bool bFlash, zxbyte Border, uTInt sx_width, uTInt sy_height, uTDisplayMode displayMode) {
	static zxbyte OldBorder=8;
	//Реализация под 32bpp
	uTUint32 BorderColor;
	uTUint32 InkColor;
	uTUint32 PaperColor;
	int i,j,k;
	int LinesInKeyboard=0;
	
	uTInt bx, by, sx, sy, px, py;
	
	zxbyte mbyte;
	zxbyte abyte;

	zxbyte* hGfx=(zxbyte*)hScrMem;
	zxbyte* hAtrs=hGfx+6144;
	
	if (sx_width<sy_height) {
		//вертикальное расположение
		bx=0;//x-пиксель начала координат картинки, верхний левый угол эмулятора, для вертикального режима - самый правый на картинке
		by=1-sx_width;//y-пиксель начала координат
		sx=sy_height; //т.к. экран повернут, то ширина окна эмуля=высоте
		sy=sx_width;  //а высота - ширине
		px=sx_width;  //чтобы сдвинуться на 1 пиксель по оси x прибавляем px
		py=-1;        //а по оси y - прибавляем py
	} else {
		//горизонтальное расположение
		bx=0;         
		by=0;
		sx=sx_width; 
		sy=sy_height;
		px=1;
		py=sx_width;
	};

	uTUint32* dMEM=(uTUint32*)MEM + bx*px + by*py;	//реализация под 32bpp

	uTInt UpBorderLines=(48-LinesInKeyboard)/2; //TODO: 48 заменить на расчет
	uTInt DownBorderLines=48-(48-LinesInKeyboard)/2;

	//BorderColor=CalcColor(Border,displayMode); 
	BorderColor=CalcColor(Border,displayMode); 
	if (OldBorder==Border) {
		dMEM+=UpBorderLines*py;
	} else {
		for (j=UpBorderLines;j--!=0;) {
			for (i=sx; i--!=0; dMEM+=px) *dMEM=BorderColor;
			dMEM+=-px*sx+py;
		};
	};
	
	BorderColor=CalcColor(Border,displayMode); //!!УДАЛИТЬ
	for (k=0;k<3;k++) {
		for (j=0;j<64;j++) {
			for (i=32; i--!=0; dMEM+=px) *dMEM=BorderColor; //кусочек бордюра
			for (i=0;i<32;i++) {
				mbyte=*(hGfx++);
				abyte=*(hAtrs++);
				InkColor=CalcColor((abyte&7)|((abyte&64)>>3),displayMode);
				PaperColor=CalcColor((abyte>>3)&15,displayMode);
				if ((bFlash)&&((abyte&128)!=0)) mbyte^=255;
				*(dMEM)=(mbyte&128)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&64)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&32)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&16)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&8)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&4)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&2)?InkColor:PaperColor;
				*(dMEM+=px)=(mbyte&1)?InkColor:PaperColor;
				dMEM+=px;
			};
			for (i=32; i--!=0; dMEM+=px) *dMEM=BorderColor;
			dMEM+=-px*sx+py;
		if ((j&7)==7) hGfx-=256*7; else { hGfx+=256-32; hAtrs-=32; };
		};
	hGfx+=2048-32*8;
	};
	BorderColor=CalcColor(Border,displayMode); //!!УДАЛИТЬ
	if (OldBorder==Border) {
		dMEM+=DownBorderLines*py;
	} else {
		for (j=DownBorderLines;j--!=0;) {
			for (i=sx; i--!=0; dMEM+=px) *dMEM=BorderColor;
			dMEM+=-px*sx+py;
		};
	};
	
	//OldBorder=Border; //РАССКОМЕНТИРОВАТЬ - не обновляется без нужды бордер сверху и снизу

};

