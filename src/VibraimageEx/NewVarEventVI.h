#pragma once
#include "NewVarEvent.h"

class CNewVarEventVI :
	public CNewVarEvent
{
public:
	CNewVarEventVI();
	~CNewVarEventVI();

	virtual int getCore();
};

