#pragma once
class CCrushCheck
{
public:
	CCrushCheck();
	~CCrushCheck();

	static ULONGLONG m_tStart;

	void Start(int bStartSisabled=0);
	static void disable();
protected:
	static int m_disabled;
	static int exception_handler(LPEXCEPTION_POINTERS p);
	static int runtime_check_handler(int errorType, const char *filename, int linenumber, const char *moduleName, const char *format, ...);
};

