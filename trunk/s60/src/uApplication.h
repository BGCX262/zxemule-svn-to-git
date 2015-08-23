/*
 * uApplication.h
 *
 *  Created on: 08.11.2009
 *      Author: capry
 */

#ifndef UAPPLICATION_H_
#define UAPPLICATION_H_

#include "uSystem.h"
#include "uTypes.h"

extern CuSystem* uSys;

void uAppUpdateDisplay( uTUint32 addr ,uTInt Width, uTInt Height, uTDisplayMode displayMode);
void uAppInit(CuSystem* salInst);
void uAppTimers(void);
uTBool uAppInput(uTInt Event, uTInt Scancode);

#endif /* SALMAIN_H_ */
