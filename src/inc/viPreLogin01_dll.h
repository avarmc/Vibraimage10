#pragma once


#ifdef VIPRELOGIN01_EXPORTS
#	define VIPRELOGIN01_API __declspec(dllexport)
#else
#	define VIPRELOGIN01_API __declspec(dllimport)
#endif


int VIPRELOGIN01_API vi_prelogin_create(CWnd** ppWnd);
void VIPRELOGIN01_API vi_prelogin_free(int id);
int VIPRELOGIN01_API vi_prelogin_call(int id, wchar_t* pLogin, wchar_t* pPwnMD5);