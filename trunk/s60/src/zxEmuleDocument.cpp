/*
 ============================================================================
 Name		: zxEmuleDocument.cpp
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : CzxEmuleDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "zxEmuleAppUi.h"
#include "zxEmuleDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CzxEmuleDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CzxEmuleDocument* CzxEmuleDocument::NewL(CEikApplication& aApp)
	{
	CzxEmuleDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CzxEmuleDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CzxEmuleDocument* CzxEmuleDocument::NewLC(CEikApplication& aApp)
	{
	CzxEmuleDocument* self = new (ELeave) CzxEmuleDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CzxEmuleDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CzxEmuleDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CzxEmuleDocument::CzxEmuleDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CzxEmuleDocument::CzxEmuleDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CzxEmuleDocument::~CzxEmuleDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CzxEmuleDocument::~CzxEmuleDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CzxEmuleDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CzxEmuleDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CzxEmuleAppUi;
	}

// End of File
