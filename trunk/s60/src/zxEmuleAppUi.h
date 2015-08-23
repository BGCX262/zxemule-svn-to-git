/*
 ============================================================================
 Name		: zxEmuleAppUi.h
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Declares UI class for application.
 ============================================================================
 */

#ifndef __ZXEMULEAPPUI_h__
#define __ZXEMULEAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CzxEmuleAppView;

// CLASS DECLARATION
/**
 * CzxEmuleAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CzxEmuleAppUi : public CAknAppUi
	{
public:
	// Constructors and destructor

	/**
	 * ConstructL.
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CzxEmuleAppUi.
	 * C++ default constructor. This needs to be public due to
	 * the way the framework constructs the AppUi
	 */
	CzxEmuleAppUi();

	/**
	 * ~CzxEmuleAppUi.
	 * Virtual Destructor.
	 */
	virtual ~CzxEmuleAppUi();
	
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange();

	/**
	 *  From CCoeAppUi, HelpContextL.
	 *  Provides help context for the application.
	 *  size is changed.
	 */
	CArrayFix<TCoeHelpContext>* HelpContextL() const;
	
	

private:
	// Data

	/**
	 * The application view
	 * Owned by CzxEmuleAppUi
	 */
	CzxEmuleAppView* iAppView;

	};

#endif // __ZXEMULEAPPUI_h__
// End of File
