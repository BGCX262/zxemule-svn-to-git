/*
 ============================================================================
 Name		: zxEmuleAppView.cpp
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <aknview.h>
#include <AknUtils.h>
#include <akndef.h>
#include <eikapp.h>
#include <eikappui.h>
//#include "bitmapmethods.h"
#include <fbs.h>
#include <gdi.h>
#include <bitdev.h>
#include "uSystem.h"
#include "uApplication.h"
#include "zxEmuleAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CzxEmuleAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CzxEmuleAppView* CzxEmuleAppView::NewL(const TRect& aRect)
	{
	CzxEmuleAppView* self = CzxEmuleAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CzxEmuleAppView* CzxEmuleAppView::NewLC(const TRect& aRect)
	{
	CzxEmuleAppView* self = new (ELeave) CzxEmuleAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Create a periodic timer but don't start it yet
	iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
	
	// Create uSystem
	iuSystem = CuSystem::NewL();
	    
	// Set the windows size
	SetRect(aRect);
	CreateOffScreenBitmap();
	
	// Activate the window, which makes it ready to be drawn
	ActivateL();
	StartTimer();
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::CzxEmuleAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CzxEmuleAppView::CzxEmuleAppView()
	: iOffScreenBitmap(NULL)
    , iOffScreenBitmapDevice(NULL)
    , iOffScreenBitmapGc(NULL)
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::~CzxEmuleAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CzxEmuleAppView::~CzxEmuleAppView()
	{
	if ( iPeriodicTimer ) {
		// Stop the periodic timer
		iPeriodicTimer->Cancel();
	}
	delete iPeriodicTimer;
	iPeriodicTimer = NULL;
	
	delete iOffScreenBitmapGc;
	iOffScreenBitmapGc = NULL;

	delete iOffScreenBitmapDevice;
	iOffScreenBitmapDevice = NULL;

	delete iOffScreenBitmap;
	iOffScreenBitmap = NULL;
	
	delete iuSystem;
	iuSystem = NULL;

	// No implementation required
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	//CWindowGc& gc = SystemGc();

	// Gets the control's extent
	//TRect drawRect(Rect());

	// Clears the screen
	//gc.Clear(drawRect);
	
	UpdateDisplay();

	}

// -----------------------------------------------------------------------------
// CGraphicsAppView::UpdateDisplay()
// Updates the display.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::UpdateDisplay() const
    {
    CWindowGc& gc = SystemGc();

    iOffScreenBitmap->LockHeap();
    // Изменяем рисунок
    TSize bitmapSize = iOffScreenBitmap->SizeInPixels();
    
    //TODO: Выделить в отдельную процедуру, и не вызывать при каждом запуске!
    uTDisplayMode displayMode;
    //выделить uEColor64K и прочие в #Defines чтобы убрать этот идиотский switch
    switch(iOffScreenBitmap->DisplayMode()) {
    	case EColor64K:		displayMode=uEColor64K;	break;
    	case EColor16MU:	displayMode=uEColor16MU;	break;
    	default:			displayMode=uENone;	break;
    };
    
    uAppUpdateDisplay((uTUint32)iOffScreenBitmap->DataAddress(),
    		(uTInt)bitmapSize.iWidth,(uTInt)bitmapSize.iHeight,displayMode);
    // Разблокируем динамическую память 
    iOffScreenBitmap->UnlockHeap();

    
    
        // Blit the background image onto the off screen bitmap
		// at the top left position
        //iOffScreenBitmapGc->BitBlt( TPoint( 0,0 ), iBackgroundImage );

        // Blit the sprites on top of it using its mask to retain
		// the background where necessary
        //for ( TInt count = 0; count<iSprites->Count(); ++count )
        //    {
        //    CSprite *sprite = iSprites->At(count);
        //    NBitmapMethods::BitBltMaskedEntireBitmap( *iOffScreenBitmapGc,
        //                                    sprite->Position(),
        //                                    *(sprite->Image()),
        //                                    *(sprite->Mask()));
        //    }

        // Blit the offscreen image onto the screen at the top left position
        gc.BitBlt( Rect().iTl,iOffScreenBitmap );
        
    }


// -----------------------------------------------------------------------------
// CzxEmuleAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	};


// -----------------------------------------------------------------------------
// Handle resource change event, here it is the event that occurs
// when screen mode changes.
// -----------------------------------------------------------------------------
void CzxEmuleAppView::HandleResourceChange(TInt aType) {
	CCoeControl::HandleResourceChange(aType);
	if( aType == KEikDynamicLayoutVariantSwitch) {
        SetExtentToWholeScreen();
        CreateOffScreenBitmap();
		}
    };

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CzxEmuleAppView::CreateOffScreenBitmap() {
    	delete iOffScreenBitmapGc;
    	iOffScreenBitmapGc = NULL;
    	delete iOffScreenBitmapDevice;
    	iOffScreenBitmapDevice = NULL;
    	delete iOffScreenBitmap;
    	iOffScreenBitmap = NULL;

        // Create the off screen bitmap and device / gc
        //iOffScreenBitmap = NBitmapMethods::CreateBitmapL( Size(), KColourDepth );
        //iOffScreenBitmapDevice = NBitmapMethods::CreateBitmapDeviceL( *iOffScreenBitmap );
        //iOffScreenBitmapGc = NBitmapMethods::CreateGraphicsContextL( *iOffScreenBitmapDevice );
        
    	// Создаем новое изображение совпадающее
        // по размеру и цветовой палитре м экраном
    	TDisplayMode displayMode = CEikonEnv::Static()->ScreenDevice()->DisplayMode();
    	iOffScreenBitmap = new(ELeave) CFbsBitmap();
    	User::LeaveIfError(iOffScreenBitmap->Create(Rect().Size(), displayMode));
    	// Создаем устройство изображения
    	iOffScreenBitmapDevice = CFbsBitmapDevice::NewL(iOffScreenBitmap);
    	// Создаем графический контекст для буферного изображения
    	User::LeaveIfError(iOffScreenBitmapDevice->CreateContext(iOffScreenBitmapGc));
}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::StopTimer()
// Stops the balls.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::StopTimer()
	{
	// Stop the timer if it is active
	if ( iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// CzxEmuleAppView::StartTimer()
// Start Timer.
// -----------------------------------------------------------------------------
//
void CzxEmuleAppView::StartTimer()
	{
	// If the timer is not already running, start it
	if ( !iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Start(1000000/75, 1000000/75, TCallBack( CzxEmuleAppView::Period, this));
		}

	}

void CzxEmuleAppView::DoPeriodTask()
	{
	// Update the screen
	CWindowGc& gc = SystemGc();
	gc.Activate( *DrawableWindow() );
	UpdateDisplay();
	gc.Deactivate();
	uAppTimers();
	}

TInt CzxEmuleAppView::Period( TAny* aPtr )
	{
    ( static_cast<CzxEmuleAppView*>( aPtr ) )->DoPeriodTask();
    // Returning a value of TRUE indicates the callback should be done again
	return ETrue;
	}


// End of File
