/*
 ============================================================================
 Name		: zxEmule.cpp
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "zxEmuleApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CzxEmuleApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

