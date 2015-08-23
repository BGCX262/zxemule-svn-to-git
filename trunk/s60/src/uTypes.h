/*
 * uTypes.h
 *
 *  Created on: 12.11.2009
 *      Author: capry
 */

#ifndef UTYPES_H_
#define UTYPES_H_

#include <e32keys.h>
#include <W32STD.H>

//TODO: сделать проверку типов!!! т.е. zxbyte,zxsbyte, uTUint8 и uTInt8 равны 1 байту! 


typedef char CHAR;
typedef CHAR *LPSTR, *PSTR;
typedef LPSTR PTSTR, LPTSTR;

/** Signed integer type of the natural machine word length. */
typedef signed int uTInt;

/** Unsigned integer type of the natural machine word length. */
typedef unsigned int uTUint;

/** 8-bit signed integer type, used in Symbian OS to mean an 8-bit
signed integer, independent of the implementation. */
typedef signed char uTInt8;

/** 8-bit unsigned integer type; used in Symbian OS to mean an 8-bit
unsigned integer, independent of the implementation. */
typedef unsigned char uTUint8;

/** 16-bit signed integer type, used in Symbian OS to mean a 16-bit
signed integer, independent of the implementation. */
typedef short int uTInt16;

/** 16-bit unsigned integer type. used in Symbian OS to mean a 16-bit
unsigned integer, independent of the implementation. */
typedef unsigned short int uTUint16;

/** 32-bit signed integer type, used in Symbian OS to mean a 32-bit
signed integer, independent of the implementation. */
typedef long int uTInt32;

/** 32-bit unsigned integer type; used in Symbian OS to mean a 32-bit
unsigned integer, independent of the implementation. */
typedef unsigned long int uTUint32;


/** 32-bit floating point number, providing IEEE754 single precision on all Symbian 
OS implementations. */
typedef float uTReal32;

/** 64-bit floating point number, providing IEEE754 double precision on all Symbian 
OS implementations. */
typedef double uTReal64;

/** 64-bit floating point number; identical to TReal64. */
typedef double uTReal;

/** 8-bit unsigned character. */
typedef unsigned char uTText8;

/** 16-bit unsigned character. */
typedef unsigned short int uTText16;

/** Boolean type which takes the value either ETrue or EFalse. */
typedef int uTBool;

typedef		unsigned char	zxbyte;
typedef		signed char		zxsbyte;
typedef		unsigned short	zxword;
typedef		signed short	zxsword;
typedef		uTInt32		zxsdword;

/*
#ifndef TVideoMode
enum TVideoMode
	{
	EMono=1,
	EGray4=2,
	EGray16=4,
	EColor256=8,
	EColor4K=12,
	EColor64K=16,
	EColor16M=24
	};
#endif
*/

/** Display modes. */

enum uTDisplayMode
	{
	/** No display mode */
	uENone,
	/** Monochrome display mode (1 bpp) */
	uEGray2,
	/** Four grayscales display mode (2 bpp) */
	uEGray4,
	/** 16 grayscales display mode (4 bpp) */
	uEGray16,
	/** 256 grayscales display mode (8 bpp) */
	uEGray256,
	/** Low colour EGA 16 colour display mode (4 bpp) */
	uEColor16,
	/** 256 colour display mode (8 bpp) */
	uEColor256,
	/** 64,000 colour display mode (16 bpp) */
	uEColor64K,
	/** True colour display mode (24 bpp) */
	uEColor16M,
	/** (Not an actual display mode used for moving buffers containing bitmaps) */
	uERgb,
	/** 4096 colour display (12 bpp). */
	uEColor4K,
	/** True colour display mode (32 bpp, but top byte is unused and unspecified) */
	uEColor16MU,
	/** Display mode with alpha (24bpp colour plus 8bpp alpha) */
	uEColor16MA,
	//Any new display mode should be insterted here!
	//There might be up to 255 display modes, so value of the last
	//inserted EColorXXX enum item should be less than 256 -
	//BC reasons!
	uEColorLast
	};

//key events
#define uEventKeyDown EEventKeyDown
#define uEventKeyUp   EEventKeyUp

//keys
#define uKeyUp      EStdKeyUpArrow
#define uKeyDown    EStdKeyDownArrow
#define uKeyLeft    EStdKeyLeftArrow
#define uKeyRight   EStdKeyRightArrow
#define uKeySelect  EStdKeyDevice3
#define uKeyBackspace EStdKeyBackspace

#define uKeyEnter	EStdKeyEnter
#define uKeySpace	EStdKeySpace

#define uKeyComma		EStdKeyComma //Scan code for Comma (,) key.
#define uKeyFullStop	EStdKeyFullStop //Scan code for Full stop (.) key.

#define uKeyLeftShift  EStdKeyLeftShift 
#define uKeyRightShift EStdKeyRightShift 
#define uKeyLeftAlt    EStdKeyLeftAlt 
#define uKeyRightAlt   EStdKeyRightAlt 
#define uKeyLeftCtrl   EStdKeyLeftCtrl 
#define uKeyRightCtrl  EStdKeyRightCtrl 
 
#define uEndOfKeys	0

#endif /* UTYPES_H_ */
