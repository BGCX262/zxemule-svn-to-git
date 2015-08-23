// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__964E2719_0092_4480_ACB4_24BEC491DEA9__INCLUDED_)
#define AFX_STDAFX_H__964E2719_0092_4480_ACB4_24BEC491DEA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#if (_WIN32_WCE <= 211)
//#error This project can not be built for H/PC Pro 2.11 or earlier platforms.
//#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// Local Header Files

typedef		unsigned char	zxbyte;
typedef		unsigned short	zxword;
typedef		signed char		zxsbyte;
typedef		signed short	zxsword;
typedef		signed int		zxsdword;

typedef		signed char			ays8;
typedef		unsigned char		ayu8;
typedef		signed short		ays16;
typedef		unsigned short		ayu16;
typedef		signed long			ays32;
typedef		unsigned long		ayu32;

const int SampleRate=44100; //11025; //22050; //44100;
//const int AYStepsInSample=5; //1750000/8/44100;    //ay.h
const int AYStepsInSampleFloat=1750000*256/8/SampleRate; //слово, нижний байт - дробная часть.
const int SamplesInFrame=SampleRate/50; //44100/50;      //ay.h
const BUFFERLENGTH=2*2*SamplesInFrame; //wave.h

#include "ZxEmulSrc\wave.h"
#include "ZxEmulSrc\ay.h"


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__964E2719_0092_4480_ACB4_24BEC491DEA9__INCLUDED_)
