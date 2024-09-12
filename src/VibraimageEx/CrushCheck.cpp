#include "stdafx.h"
#include "CrushCheck.h"
#include <rtcapi.h>
#include "VibraimageEx.h"

ULONGLONG CCrushCheck::m_tStart = GetTickCount64();

int CCrushCheck::m_disabled = 1;

CCrushCheck::CCrushCheck()
{

}


CCrushCheck::~CCrushCheck()
{
}


void CCrushCheck::disable()
{
	m_disabled = 1;
}

void CCrushCheck::Start(int bStartSisabled)
{
	m_disabled = bStartSisabled;

	DWORD dwMode = SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetErrorMode(dwMode | SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)&exception_handler);
	_RTC_SetErrorFunc(&runtime_check_handler);
}

int CCrushCheck::exception_handler(LPEXCEPTION_POINTERS p)
{
	theApp.m_splashScreen.Finish();
	WCHAR msg[2048];
	_snwprintf_s(msg, 2048, 2027, L"unhandled exception [%0.8X]", p->ExceptionRecord->ExceptionCode);
	theApp.OnError(-1, EVENTLOG_ERROR_TYPE, msg);

	if (m_disabled || theApp.m_bDone || theApp.m_bCloseRqst)
		return 0;
	theApp.m_bReady = 0;
	ULONGLONG dt = GetTickCount64()- m_tStart;
	if(dt > 5000)
		theApp.Restart();
	return 0;
}

int CCrushCheck::runtime_check_handler(int errorType, const char *filename, int linenumber, const char *moduleName, const char *format, ...)
{
	theApp.m_splashScreen.Finish();

	va_list argList;
	va_start(argList, format);
	char msg[2048],tmp[2048];
	ZeroMemory(msg, sizeof(msg));
	ZeroMemory(tmp, sizeof(tmp));
	_snprintf_s(tmp, 2048, 2027, format, argList);
	_snprintf_s(msg, 2048, 2027, "%s line=%d type=%d module=[%s] {%s}", filename, linenumber, errorType, moduleName, tmp);
	va_end(argList);

	theApp.OnError(-1, EVENTLOG_ERROR_TYPE, CString(msg));


	if (m_disabled || theApp.m_bDone || theApp.m_bCloseRqst)
		return 0;
	theApp.m_bReady = 0;
	ULONGLONG dt = GetTickCount64() - m_tStart;
	if (dt > 5000)
		theApp.Restart();
	return 0;
}