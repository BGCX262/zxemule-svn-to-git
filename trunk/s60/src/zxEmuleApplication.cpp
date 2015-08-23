/*
 ============================================================================
 Name		: zxEmuleApplication.cpp
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "zxEmule.hrh"
#include "zxEmuleDocument.h"
#include "zxEmuleApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CzxEmuleApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CzxEmuleApplication::CreateDocumentL()
	{
	// Create an zxEmule document, and return a pointer to it
	return CzxEmuleDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CzxEmuleApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CzxEmuleApplication::AppDllUid() const
	{
	// Return the UID for the zxEmule application
	return KUidzxEmuleApp;
	}

// End of File
