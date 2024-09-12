#pragma once
#include "VIVarTag.h"



typedef struct tagVI_VAR
{
	float		fv1 = 0;
	float		fv2 = 0;
	int			iv1 = 0;
	int			iv2 = 0;
} VI_VAR;

enum VI_VAR_SUB
{
	VI_VAR_SUB_I1,VI_VAR_SUB_I2,VI_VAR_SUB_F1,VI_VAR_SUB_F2,
	VI_VAR_SUB_STRING,
	VI_VAR_SUB_BOOL1, VI_VAR_SUB_BOOL2,
	VI_VAR_SUB_SELECT1, VI_VAR_SUB_SELECT2
};

