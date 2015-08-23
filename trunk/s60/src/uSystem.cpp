/*
 ============================================================================
 Name		: uSystem.cpp
 Author	  : tsapree
 Version	 : 1.0
 Copyright   : (c) tsap 2009
 Description : CuSystem implementation, S60 edition. SystemAbstractionLayer. 
 ============================================================================
 */
// TODO: � ���� ������ ���� ����� ������������� ��� ������, �������
// ����� �������������� �����������. 
#include "uSystem.h"
#include "uApplication.h"

CuSystem::CuSystem()
	{
	// No implementation required
	}

CuSystem::~CuSystem()
	{
	}

CuSystem* CuSystem::NewLC()
	{
	CuSystem* self =
			new (ELeave) CuSystem();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CuSystem* CuSystem::NewL()
	{
	CuSystem* self = CuSystem::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CuSystem::ConstructL()
	{
	uAppInit(this);
	}

void CuSystem::uFileOpen(LPTSTR filename) {
	bfile = fopen(filename,"rb"); //TODO: ������� �������� �� �����... ���� - ���������
}

void CuSystem::uFileRead(void* addr,uTUint len) {
	if (bfile!=0) fread(addr,len,1,bfile);
}

void CuSystem::uFileClose(void) {
	if (bfile!=0) fclose(bfile);
	bfile=0;
}

int CuSystem::uFileEOF(void) {
//TODO: ���������, ����� �� ����� ���� ������ ���������� ��������� ���� ����� �����
	return feof(bfile);
}


