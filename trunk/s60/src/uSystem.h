/*
 ============================================================================
 Name		: uSystem.h
 Author	  : tsapree
 Version	 : 1.0
 Copyright   : (c) tsap 2009
 Description : CuSystem declaration.
 ============================================================================
 */

#ifndef USYSTEM_H
#define USYSTEM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stdio.h> //скопировать в include из libc
#include "uTypes.h"


// CLASS DECLARATION

/**
 *  CuSystem
 * 
 */
class CuSystem : public CBase
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CuSystem();

	/**
	 * Two-phased constructor.
	 */
	static CuSystem* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CuSystem* NewLC();
	
	void uFileOpen(LPTSTR filename);
	void uFileRead(void* addr,uTUint len);
	void uFileClose(void);
	int uFileEOF(void);

private:
	FILE* bfile;

	/**
	 * Constructor for performing 1st stage construction
	 */
	CuSystem();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

	};

#endif // USYSTEM_H
