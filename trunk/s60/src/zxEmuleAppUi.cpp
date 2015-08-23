/*
 ============================================================================
 Name		: zxEmuleAppUi.cpp
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : CzxEmuleAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <zxEmule_0xE9368B92.rsg>

#include "zxEmule_0xE9368B92.hlp.hrh"
#include "zxEmule.hrh"
#include "zxEmule.pan"
#include "zxEmuleApplication.h"
#include "zxEmuleAppUi.h"
#include "zxEmuleAppView.h"
#include "uApplication.h"
#include "uTypes.h"

#include "Z80.h" //УБРАТЬ ПОСЛЕ отделения uApplication от системы

_LIT( KFileName, "C:\\private\\E9368B92\\zxEmule.txt" );
_LIT( KText, "Hello World!");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CzxEmuleAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	//iAppView = CzxEmuleAppView::NewL(ClientRect());
	iAppView = CzxEmuleAppView::NewL(ApplicationRect());
	AddToStackL( iAppView ); //ВАЖНО для вызова HandleResourceChange при смене настроек экрана

	// Create a file to write the text to
	TInt err = CCoeEnv::Static()->FsSession().MkDirAll(KFileName);
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		return;
		}

	RFile file;
	err = file.Replace(CCoeEnv::Static()->FsSession(), KFileName, EFileWrite);
	CleanupClosePushL(file);
	if (KErrNone != err)
		{
		CleanupStack::PopAndDestroy(1); // file
		return;
		}

	RFileWriteStream outputFileStream(file);
	CleanupClosePushL(outputFileStream);
	outputFileStream << KText;

	CleanupStack::PopAndDestroy(2); // outputFileStream, file

	}
// -----------------------------------------------------------------------------
// CzxEmuleAppUi::CzxEmuleAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CzxEmuleAppUi::CzxEmuleAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppUi::~CzxEmuleAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CzxEmuleAppUi::~CzxEmuleAppUi()
	{
	if (iAppView)
		{
		iEikonEnv->RemoveFromStack( iAppView );
		delete iAppView;
		iAppView = NULL;
		}

	}

// -----------------------------------------------------------------------------
// CzxEmuleAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		case ECommand1:
			{

			// Load a string from the resource file and display it
			HBufC* textResource = StringLoader::LoadLC(R_COMMAND1_TEXT);
			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;

			// Show the information Note with
			// textResource loaded with StringLoader.
			informationNote->ExecuteLD(*textResource);

			// Pop HBuf from CleanUpStack and Destroy it.
			CleanupStack::PopAndDestroy(textResource);
			}
			break;
		case ECommand2:
			{
			RFile rFile;

			//Open file where the stream text is
			User::LeaveIfError(rFile.Open(CCoeEnv::Static()->FsSession(),
					KFileName, EFileStreamText));//EFileShareReadersOnly));// EFileStreamText));
			CleanupClosePushL(rFile);

			// copy stream from file to RFileStream object
			RFileReadStream inputFileStream(rFile);
			CleanupClosePushL(inputFileStream);

			// HBufC descriptor is created from the RFileStream object.
			HBufC* fileData = HBufC::NewLC(inputFileStream, 32);

			CAknInformationNote* informationNote;

			informationNote = new (ELeave) CAknInformationNote;
			// Show the information Note
			informationNote->ExecuteLD(*fileData);

			// Pop loaded resources from the cleanup stack
			CleanupStack::PopAndDestroy(3); // filedata, inputFileStream, rFile
			}
			break;
		case EReset:
			ResetZX();
			OutPortZX(0x7ffd,7+16);
			setPC(0); //pc=PopZX();
			EnableTrdos();
			break;
		case EHelp:
			{

			CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
			HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
			}
			break;
		case EAbout:
			{

			CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog();
			dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
			HBufC* title = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
			dlg->QueryHeading()->SetTextL(*title);
			CleanupStack::PopAndDestroy(); //title
			HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
			dlg->SetMessageTextL(*msg);
			CleanupStack::PopAndDestroy(); //msg
			dlg->RunLD();
			}
			break;
		default:
			Panic(EzxEmuleUi);
			break;
		}
	}


TKeyResponse CzxEmuleAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
   {
   if (uAppInput((uTInt)aType, (uTInt)aKeyEvent.iScanCode)) return EKeyWasConsumed;
   else             return EKeyWasNotConsumed;

   }

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CzxEmuleAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect());
	}

CArrayFix<TCoeHelpContext>* CzxEmuleAppUi::HelpContextL() const
	{
#warning "Please see comment about help and UID3..."
	// Note: Help will not work if the application uid3 is not in the
	// protected range.  The default uid3 range for projects created
	// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
	// can be self signed and installed on the device during testing.
	// Once you get your official uid3 from Symbian Ltd. and find/replace
	// all occurrences of uid3 in your project, the context help will
	// work. Alternatively, a patch now exists for the versions of 
	// HTML help compiler in SDKs and can be found here along with an FAQ:
	// http://www3.symbian.com/faq.nsf/AllByDate/E9DF3257FD565A658025733900805EA2?OpenDocument
	CArrayFixFlat<TCoeHelpContext>* array = new (ELeave) CArrayFixFlat<
			TCoeHelpContext> (1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidzxEmuleApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;
	}

// End of File
