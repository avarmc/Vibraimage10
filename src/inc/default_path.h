#pragma once

#include <filesystem>

inline CString GetDefaultPath()
{
	TCHAR fileName[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, fileName, MAX_PATH + 1);

	CString path = std::filesystem::absolute(fileName).c_str();

	int pos = path.ReverseFind('\\');
	if (pos >= 0)
		return path.Left(pos + 1);
	return path;
}