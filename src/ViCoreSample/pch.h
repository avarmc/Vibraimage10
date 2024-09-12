#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <filesystem>
#include <tchar.h>

#ifndef _T
#	define _T(x)      L ## x
#endif

#include "VIEngine.h"
#include "resource_cnd.h"
