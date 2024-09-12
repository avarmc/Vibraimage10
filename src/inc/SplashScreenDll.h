#pragma once

#include "default_path.h"
#include <vector>

class CSplashScreenLock {
	HANDLE hM;
public:
	CSplashScreenLock() {
		hM = INVALID_HANDLE_VALUE;
		Lock();
	}
	~CSplashScreenLock() {
		Unlock();
	}

	void Lock() {
		hM = CreateMutex(NULL, FALSE, L"Global\\Vibraimage_Starter_Lock");
		if (!hM || hM == INVALID_HANDLE_VALUE) {
//			AfxMessageBox(L"create FAIL");
			return;
		}


		int dwWaitResult = WaitForSingleObject(hM, INFINITE);
		if (dwWaitResult == WAIT_OBJECT_0) {
			// The thread got ownership of the mutex
//			AfxMessageBox(L"create Owner");
			return;
		}
		if (dwWaitResult == WAIT_ABANDONED) {
			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
//			AfxMessageBox(L"create ABANDONED");
			return;
		}
	}

	void Unlock() {
		if (hM && hM != INVALID_HANDLE_VALUE) {
//			AfxMessageBox(L"unlock ok");

			ReleaseMutex(hM);
			CloseHandle(hM);
			hM = INVALID_HANDLE_VALUE;	
		} 
	}
};
class CSplashScreen {

	CString filePath;

public:
	CSplashScreen(bool bStart) {
#ifndef _DEBUG
		if (bStart)
			Start();
#endif
	}

	~CSplashScreen() {
		Finish();
	}

	void Start() {
#ifdef _DEBUG
		return;
#endif
		if (!filePath.IsEmpty())
			return;

		TCHAR lpTempPathBuffer[MAX_PATH];
		TCHAR szTempFileName[MAX_PATH];
		TCHAR szShortFileName[MAX_PATH] = { 0 };
		DWORD dwRetVal = 0;

		CString dll_path = GetDefaultPath() + L"SplashScreen.dll";
		CString cmd;


		dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
			lpTempPathBuffer); // buffer for path 
		if (dwRetVal > MAX_PATH || (dwRetVal == 0))
			return;
		dwRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
			TEXT("VISS"),     // temp file name prefix 
			1,                // create unique name 
			szTempFileName);  // buffer for name 
		if (dwRetVal == 0)
			return;

		CStringA pid;
		pid.Format("%u", GetCurrentProcessId());

		BOOL ok = FALSE;
		try {
			CFile   file;
			ok = file.Open(szTempFileName, CFile::modeCreate | CFile::modeReadWrite);
			file.Write((LPCSTR)pid, pid.GetLength());
			file.Close();
		}
		catch (CFileException*) {

		}

		if (ok) {
			dwRetVal = GetShortPathName(szTempFileName, szShortFileName, MAX_PATH);
			if (dwRetVal == 0)
				return;

			filePath = szShortFileName;
			cmd.Format(L"\"%s\",ShowSplashScreen \"%s\"", (LPCTSTR)dll_path, szShortFileName);

			ShellExecute(NULL, NULL, L"rundll32", cmd, NULL, SW_SHOWNORMAL);
		}
	}

	void Finish() {
#ifdef _DEBUG
		return;
#endif
		if (filePath.IsEmpty())
			return;
		try {

			CFile::Remove(filePath);
			filePath.Empty();
		}
		catch (CFileException*) {

		}
	}
};
