/*
 ============================================================================
 Name		: zxEmuleAppView.h
 Author	  : tsapree
 Copyright   : (c) tsap 2009
 Description : Declares view class for application.
 ============================================================================
 */

#ifndef __ZXEMULEAPPVIEW_h__
#define __ZXEMULEAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>
#include <eikenv.h>
#include <AknIconUtils.h>
#include "uSystem.h"


// CLASS DECLARATION
class CzxEmuleAppView : public CCoeControl
	{
public:
	// New methods

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Create a CzxEmuleAppView object, which will draw itself to aRect.
	 * @param aRect The rectangle this view will be drawn to.
	 * @return a pointer to the created instance of CzxEmuleAppView.
	 */
	static CzxEmuleAppView* NewL(const TRect& aRect);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Create a CzxEmuleAppView object, which will draw itself
	 * to aRect.
	 * @param aRect Rectangle this view will be drawn to.
	 * @return A pointer to the created instance of CzxEmuleAppView.
	 */
	static CzxEmuleAppView* NewLC(const TRect& aRect);

	/**
	 * ~CzxEmuleAppView
	 * Virtual Destructor.
	 */
	virtual ~CzxEmuleAppView();

public:
	// Functions from base classes




	/**
	 * From CoeControl, SizeChanged.
	 * Called by framework when the view size is changed.
	 */
	virtual void SizeChanged();

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs, 
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private:
	/**
	 * From CCoeControl, Draw
	 * Draw this CzxEmuleAppView to the screen.
	 * @param aRect the rectangle of this view that needs updating
	 */
	void Draw(const TRect& aRect) const;
	void HandleResourceChange(TInt aType);
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 * Perform the second phase construction of a
	 * CzxEmuleAppView object.
	 * @param aRect The rectangle this view will be drawn to.
	 */
	void ConstructL(const TRect& aRect);

	/**
	 * CzxEmuleAppView.
	 * C++ default constructor.
	 */
	CzxEmuleAppView();

private: // New functions

    /**
    * UpdateDisplay.
    * Perfom the drawing, does not activate the GC.
    */
    void UpdateDisplay() const;
    void CreateOffScreenBitmap();
    void StopTimer();
    void StartTimer();
    void DoPeriodTask();
    static TInt Period( TAny* aPtr );
	
    /**
     * iOffScreenBitmap
     * The off screen bitmap
     * Owned by CGraphicsAppView object.
     */
     CFbsBitmap* iOffScreenBitmap;

     /**
     * iOffScreenBitmapDevice
     * The off screen bitmap device
     * Owned by CGraphicsAppView object.
     */
     CFbsBitmapDevice* iOffScreenBitmapDevice;

     /**
     * iOffScreenBitmapGc
     * The off screen bitmap gc
     * Owned by CGraphicsAppView object.
     */
     CFbsBitGc* iOffScreenBitmapGc;
     
     /**
     * iPeriodicTimer
     * The periodic timer used to control the animation
     * Owned by CGraphicsAppView object.
     */
     CPeriodic* iPeriodicTimer;
     
     CuSystem* iuSystem; 
        
	};

#endif // __ZXEMULEAPPVIEW_h__
// End of File
