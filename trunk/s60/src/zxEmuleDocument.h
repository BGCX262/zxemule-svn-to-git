/*
 ============================================================================
 Name		: zxEmuleDocument.h
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __ZXEMULEDOCUMENT_h__
#define __ZXEMULEDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CzxEmuleAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CzxEmuleDocument application class.
 * An instance of class CzxEmuleDocument is the Document part of the
 * AVKON application framework for the zxEmule example application.
 */
class CzxEmuleDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CzxEmuleDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CzxEmuleDocument.
	 */
	static CzxEmuleDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CzxEmuleDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CzxEmuleDocument.
	 */
	static CzxEmuleDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CzxEmuleDocument
	 * Virtual Destructor.
	 */
	virtual ~CzxEmuleDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CzxEmuleAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CzxEmuleDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CzxEmuleDocument(CEikApplication& aApp);

	};

#endif // __ZXEMULEDOCUMENT_h__
// End of File
