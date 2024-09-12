#pragma once

#include <string>
#pragma comment(lib, "advapi32.lib")

class CLogReport
{
public:
	CLogReport();
	~CLogReport();

	bool Register(CString a_name);
	void Unregister(CString a_name);

	bool LogMessage(CString a_msg,const WORD a_type, CString a_name);
	bool IsRegistered(CString a_name);
};

