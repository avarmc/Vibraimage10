#include "stdafx.h"
#include "NewVarEventVI.h"
#include "VibraimageEx.h"

CNewVarEventVI::CNewVarEventVI()
{
}


CNewVarEventVI::~CNewVarEventVI()
{
}

int CNewVarEventVI::getCore() {
	if (!theApp.m_pEngine)
		return -1;
	return theApp.m_pEngine->id;
}