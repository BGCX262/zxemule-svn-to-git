/*
 ============================================================================
 Name		: zxEmule.pan
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __ZXEMULE_PAN__
#define __ZXEMULE_PAN__

/** zxEmule application panic codes */
enum TzxEmulePanics
	{
	EzxEmuleUi = 1
	// add further panics here
	};

inline void Panic(TzxEmulePanics aReason)
	{
	_LIT(applicationName, "zxEmule");
	User::Panic(applicationName, aReason);
	}

#endif // __ZXEMULE_PAN__
