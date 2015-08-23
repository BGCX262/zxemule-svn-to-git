/*
 ============================================================================
 Name		: zxEmuleApplication.h
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __ZXEMULEAPPLICATION_H__
#define __ZXEMULEAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "zxEmule.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidzxEmuleApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CzxEmuleApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CzxEmuleApplication is the application part of the
 * AVKON application framework for the zxEmule example application.
 */
class CzxEmuleApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidzxEmuleApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CzxEmuleDocument document object. The returned
	 * pointer in not owned by the CzxEmuleApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __ZXEMULEAPPLICATION_H__
// End of File
