#ifndef __VI_NETCTRL_H__
#define __VI_NETCTRL_H__

#include "VIVar.h"

#ifdef VINETCTRL_EXPORTS
#define VINETCTRL_API __declspec(dllexport)
#else
#define VINETCTRL_API __declspec(dllimport)
#endif

typedef void * VI_MODULE;

#define	TAG_VI_NETCTRL_VAR_STATE ""
#define	TAG_VI_NETCTRL_VAR_STATE_CRITICAL ""

/*/////////////////////////////////////////////////////////
 vi_netctrl_Connect
/////////////////////////////////////////////////////////
�������� ������ ����������� ��� ���������� IP � ��������
/////////////////////////////////////////////////////////
 VI_MODULE vi_netctrl_Connect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
/////////////////////////////////////////////////////////
module:			��������� ������, �  �������� ����������� �����������. ���� module=NULL, �� ��������� �����.
remIP:			����� ������. 
modPortTCP:		������� ���� ������
terPortTCP:		��������� TCP ����
terPortUDP:		��������� UDP ����
/////////////////////////////////////////////////////////
������� ����������� ��������� ������ ��� NULL � ������ ������
*! terPortTCP,terPortUDP,modPortTCP ����������� ������ ��� �������� ������ (module=0). ��� ���������� IP � ������ ��� ������ ��������� 0.
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_AddTag
/////////////////////////////////////////////////////////
�������� ����� ��� ������/�������� ���������
������� ������ ���� ������� ��� ������� �� ���������� ����� ������ vi_netctrl_Connect
/////////////////////////////////////////////////////////
int vi_netctrl_AddTag(VI_MODULE module,int id,const char *tag)
/////////////////////////////////////////////////////////
module:			��������� ������, �  �������� ����������� �����������
id:				�������� ������������� ��������� � ���������. (������������ ��������)
tag:			��������� ������������� ��������� � Vibraimage10
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_RqstTag
/////////////////////////////////////////////////////////
������ � ������ �� �������� ���������� ���������
������� ������ ���� ������� ��� ������� �� ���������� ����� ������ vi_netctrl_Connect
/////////////////////////////////////////////////////////
int vi_netctrl_RqstTag(VI_MODULE module,const char *tag,int mode)
/////////////////////////////////////////////////////////
module:			��������� ������, �  �������� ����������� �����������
tag:			��������� ������������� ��������� � Vibraimage10
mode:			����� ��������
					0 - � ������ ���������
					1 - ����� ��������� ���������� �����
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Test
/////////////////////////////////////////////////////////
�������� ����������� ����������
/////////////////////////////////////////////////////////
 int vi_netctrl_Test(VI_MODULE module);
/////////////////////////////////////////////////////////
module:			������� ������
/////////////////////////////////////////////////////////
������� ���������� �������� ���������� � (��������/����������) 
����������� ���������� ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Disconnect
/////////////////////////////////////////////////////////
���������� ����������
/////////////////////////////////////////////////////////
int	vi_netctrl_Disconnect(VI_MODULE module, const char *remIP);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			�� ������ ����� ������ ��������������� �����. ���� remIP=NULL ������ ��������� ��������� ������.
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetState
/////////////////////////////////////////////////////////
������������ � ����������� ������ ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_GetState(VI_MODULE module, const char *remIP, double *pState, double *pMax, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ����� �������� ��������� �������� �������� � ������ ������.
pState:			������������ � ������ ��������� (����� ���� 0)
pMax:			����������� ������� ��������� (����� ���� 0)
pVer:			����� �� �������� ����� ��������� ����� ���������. (������������� ((ULONG)GetTickCount64())). (����� ���� 0)
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
��� �������� ������ ������ ���� ������ ����� ���
TAG_VI_NETCTRL_VAR_STATE � TAG_VI_NETCTRL_VAR_STATE_CRITICAL
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetVar
/////////////////////////////////////////////////////////
��������� ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_GetVar(VI_MODULE module, const char *remIP, int idVar, VI_VAR *pVar, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ����� �������� ��������� �������� �������� � ������ ������.
idVar:			����� �������������� ���������
pVar:			����� �� �������� ����� ��������� ��������. (����� ���� 0)
pVer:			����� �� �������� ����� ��������� ����� ���������. (������������� ((ULONG)GetTickCount64())). (����� ���� 0)
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetStr [���������������. � ������ ������ �� ������������]
/////////////////////////////////////////////////////////
��������� ���������� ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_GetStr(VI_MODULE module, const char *remIP, int idVar, char *pVar, int nMax, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ����� �������� ��������� �������� �������� � ������ ������.
idVar:			����� �������������� ���������
pVar:			����� �� �������� ����� ��������� ��������. (����� ���� 0)
nMax:			������������ ����� �������� � pVar
pVer:			����� �� �������� ����� ��������� ����� ���������. (������������� ((ULONG)GetTickCount64())). (����� ���� 0)
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_PutVar
/////////////////////////////////////////////////////////
�������� ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_PutVar(VI_MODULE module, const char *remIP, const int *idVar, const VI_VAR *pVar, int cntVar);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL �������� ����� ��������� �� ���� ��������� �������.
idVar:			������, �������� [cntVar], ���������� ������ ����������
pVar:			������, �������� [cntVar], ���������� �������� ����������
cntVer:			���������� ����������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_PutStr [���������������. � ������ ������ �� ������������]
/////////////////////////////////////////////////////////
�������� ���������� ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_PutStr(VI_MODULE module, const char *remIP, const int *idVar, const char **pVar, int cntVar);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL �������� ����� ��������� �� ���� ��������� �������.
idVar:			������, �������� [cntVar], ���������� ������ ����������
pVar:			������, �������� [cntVar], ���������� ������ �����
cntVer:			���������� ����������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetVarInfo [� ������ ������ ����������� ��������]
/////////////////////////////////////////////////////////
��������� ���������� � ���������
/////////////////////////////////////////////////////////
int	vi_netctrl_GetVarInfo(int idVar, char *name, int nMax, int *pType, VI_VAR *vMin, VI_VAR *vMax);
/////////////////////////////////////////////////////////
idVar:			����� �������������� ���������
name:			����� �� �������� ����� ��������� �������� ��������� [nMax]
nMax:			������������ ����� �������� � name
pType:			���������� ��� �������� (VI_VAR_SUB); -1 ���� �� ����������
vMin:			����������� �������� ��������� (��������������)
vMax:			������������ �������� ��������� (��������������)
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetImage
/////////////////////////////////////////////////////////
��������� �����������
/////////////////////////////////////////////////////////
HGLOBAL	vi_netctrl_GetImage(VI_MODULE module, const char *remIP, int *pW, int *pH, int *pBPP,DWORD *pVer);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ����� �������� ��������� �������� �������� � ������ ������.
pW:				������ ����������� (����� ���� 0)
pH:				������ ����������� (����� ���� 0)
pBPP:			[������ �����] ����������� (����� ���� 0)
pVer:			����� �� �������� ����� ��������� ����� ���������. (������������� ((ULONG)GetTickCount64())). (����� ���� 0)
/////////////////////////////////////////////////////////
������� ����������� HGLOBAL � ������ ��������� ���������� � 0 � ������ ������
��� ��������� ������ ������� ������������ ������� GlobalLock, GlobalUnlock
����� ���������� ������ � ������ ������� ������������ GlobalFree
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetImagePack
/////////////////////////////////////////////////////////
��������� ����������� � ����������� ���� (��� ��������� � ������)
/////////////////////////////////////////////////////////
HGLOBAL	vi_netctrl_GetImagePack(VI_MODULE module, const char *remIP,DWORD *pVer);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ����� �������� ��������� �������� �������� � ������ ������.
pVer:			����� �� �������� ����� ��������� ����� ���������. (������������� ((ULONG)GetTickCount64())). (����� ���� 0)
/////////////////////////////////////////////////////////
������� ����������� HGLOBAL � ������ ��������� ���������� � 0 � ������ ������
��� ��������� ������ ������� ������������ ������� GlobalLock, GlobalUnlock
����� ���������� ������ � ������ ������� ������������ GlobalFree
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Config
/////////////////////////////////////////////////////////
����� ���� �������� [���������������. � ������ ������ �� �����������]
/////////////////////////////////////////////////////////
int	vi_netctrl_Config(VI_MODULE module, const char *remIP, HWND hWndParent);
/////////////////////////////////////////////////////////
module:			������� ������
remIP:			����� ������. ���� remIP=NULL ��������� ����� �������� ���� �������
hWndParent:		���� ����������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnVar
/////////////////////////////////////////////////////////
��������� ����������� �� ����� ����������
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnVar(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			������� ������
pFn:			static void CallbackOnVar(void *UserData,VI_MODULE module,const char *ip,const int *idVar, const VI_VAR *pVar, int cntVar);
UserData:		������ ������������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnStr
/////////////////////////////////////////////////////////
��������� ����������� �� ����� ��������� ����������
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnStr(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			������� ������
pFn:			static void CallbackOnStr(void *UserData,VI_MODULE module,const char *ip,const int *idVar, const char **pVar, int cntVar);
UserData:		������ ������������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnImage
/////////////////////////////////////////////////////////
��������� ����������� �� ����� �����������
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnImage(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			������� ������
pFn:			static void CallbackOnImage(void *UserData,VI_MODULE module,const char *ip,void *pBits, int w, int h, int bpp);
UserData:		������ ������������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnImagePack
/////////////////////////////////////////////////////////
��������� ����������� �� ����� ����������� � ����������� ����
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnImagePack(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			������� ������
pFn:			static void CallbackOnImagePack(void *UserData,VI_MODULE module,const char *ip,void *ptr,int size,void *pData,int dataSize);
UserData:		������ ������������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetTimes
/////////////////////////////////////////////////////////
��������� ������� ���������� ��������� � ���� ( ��. ((ULONG)GetTickCount64()) )
/////////////////////////////////////////////////////////
int	vi_netctrl_GetTimes(VI_MODULE module, ULONGLONG *ptRcv,ULONGLONG *ptSnd)
/////////////////////////////////////////////////////////
module:			������� ������
ptRcv:			����� ������; ����� ���� 0
ptSend:			����� ��������; ����� ���� 0
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
������� ������ ����� (������������ ������ �� ������)
/////////////////////////////////////////////////////////
VI_MODULE vi_netctrl_TermConnect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
int	vi_netctrl_PutImage(VI_MODULE module, void *pImg, int size,void *pAdd,int sizeAdd);
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnRqst
/////////////////////////////////////////////////////////
��������� ����������� �� ����� �������� �� �������
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnRqst(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			������� ������
pFn:			static void CallbackOnVar(void *UserData,VI_MODULE module,const char *ip,const char* tag,int mode);
UserData:		������ ������������
/////////////////////////////////////////////////////////
������� ����������� 0 � ������ ��������� ����������
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
	CVINetModuleDll ������� DLL, ��������� � �����
/////////////////////////////////////////////////////////*/
class CVINetModuleDll
{
public:
	CVINetModuleDll();
	virtual ~CVINetModuleDll();
public:
	VI_MODULE Connect(const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
	int Test();
	int	Disconnect(const char *remIP);
	int	GetState(const char *remIP, double *pState, double *pMax, DWORD *pVer);
	int	GetVar(const char *remIP, int idVar, VI_VAR *pVar, DWORD *pVer);
	int	GetStr(const char *remIP, int idVar, char *pVar, int nMax, DWORD *pVer);
	int	PutVar(const char *remIP, const int *idVar, const VI_VAR *pVar, int cntVar);
	int	PutStr(const char *remIP, const int *idVar, const char **pVar, int cntVar);
	int	GetVarInfo(int idVar, char *name, int nMax, int *pType, VI_VAR *vMin, VI_VAR *vMax);
	HGLOBAL	GetImage(const char *remIP, int *pW, int *pH, int *pBPP,DWORD *pVer);
	HGLOBAL	GetImagePack(const char *remIP, DWORD *pVer);

	void SetExtVer(int ver);
	int GetExtVer();

	int	Config(const char *remIP, HWND hWndParent);
	
	int	SetCallbackOnVar(void *pFn, void *UserData);
	int	SetCallbackOnRqst(void *pFn, void *UserData);
	int	SetCallbackOnStr(void *pFn, void *UserData);
	int	SetCallbackOnImage(void *pFn, void *UserData);
	int	SetCallbackOnImagePack(void *pFn, void *UserData);

	VI_MODULE TermConnect(const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
	int	PutImage(void *pImg, int size,void *pAdd=0,int sizeAdd=0);

	int	GetTimes(ULONGLONG*ptRcv, ULONGLONG*ptSnd);
	int	AddTag(int id,const char *tag);
	int	RqstTag(const char *tag,int mode);
	int Tag2Id(const char *tag);
	const char* Id2Tag(int id);

	bool	IsOpen() { return (pvi!=0); }
	bool IsOk() { return true; }

private:
	VI_MODULE pvi;
};


VINETCTRL_API VI_MODULE vi_netctrl_Connect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
VINETCTRL_API int vi_netctrl_Test(VI_MODULE module);
VINETCTRL_API int vi_netctrl_Disconnect(VI_MODULE module, const char *remIP);
VINETCTRL_API int vi_netctrl_GetState(VI_MODULE module, const char *remIP, double *pState, double *pMax, DWORD *pVer);
VINETCTRL_API int vi_netctrl_GetVar(VI_MODULE module, const char *remIP, int idVar, VI_VAR *pVar, DWORD *pVer);
VINETCTRL_API int vi_netctrl_GetStr(VI_MODULE module, const char *remIP, int idVar, char *pVar, int nMax, DWORD *pVer);
VINETCTRL_API int vi_netctrl_PutVar(VI_MODULE module, const char *remIP, const int *idVar, const VI_VAR *pVar, int cntVar);
VINETCTRL_API int vi_netctrl_PutStr(VI_MODULE module, const char *remIP, const int *idVar, const char **pVar, int cntVar);
VINETCTRL_API int vi_netctrl_GetVarInfo(int idVar, char *name, int nMax, int *pType, VI_VAR *vMin, VI_VAR *vMax);

VINETCTRL_API int vi_netctrl_SetExtVer(VI_MODULE module, int ver);
VINETCTRL_API int vi_netctrl_GetExtVer(VI_MODULE module);

VINETCTRL_API HGLOBAL vi_netctrl_GetImage(VI_MODULE module, const char *remIP, int *pW, int *pH, int *pBPP, DWORD *pVer);
VINETCTRL_API HGLOBAL vi_netctrl_GetImagePack(VI_MODULE module, const char *remIP, DWORD *pVer);

VINETCTRL_API int vi_netctrl_Config(VI_MODULE module, const char *remIP, HWND hWndParent);

VINETCTRL_API int vi_netctrl_SetCallbackOnVar(VI_MODULE module, void *pFn, void *UserData);
VINETCTRL_API int vi_netctrl_SetCallbackOnRqst(VI_MODULE module, void *pFn, void *UserData);
VINETCTRL_API int vi_netctrl_SetCallbackOnStr(VI_MODULE module, void *pFn, void *UserData);
VINETCTRL_API int vi_netctrl_SetCallbackOnImage(VI_MODULE module, void *pFn, void *UserData);
VINETCTRL_API int vi_netctrl_SetCallbackOnImagePack(VI_MODULE module, void *pFn, void *UserData);

VINETCTRL_API VI_MODULE vi_netctrl_TermConnect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
VINETCTRL_API int vi_netctrl_PutImage(VI_MODULE module, void *pImg, int size, void *pAdd, int sizeAdd);

VINETCTRL_API int vi_netctrl_GetTimes(VI_MODULE module, ULONGLONG*ptRcv, ULONGLONG*ptSnd);
VINETCTRL_API int vi_netctrl_AddTag(VI_MODULE module, int id, const char *tag);
VINETCTRL_API int vi_netctrl_RqstTag(VI_MODULE module, const char *tag, int mode);
VINETCTRL_API int vi_netctrl_Tag2Id(VI_MODULE module, const char *tag);
VINETCTRL_API const char*  vi_netctrl_Id2Tag(VI_MODULE module, int id);

inline CVINetModuleDll::CVINetModuleDll() : pvi(0)
{
}

inline CVINetModuleDll::~CVINetModuleDll()
{
}

inline VI_MODULE CVINetModuleDll::Connect(const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP)
{
	if (!IsOk()) return 0;
	if(pvi)
		return vi_netctrl_Connect(pvi,remIP,modPortTCP,terPortTCP,terPortUDP);
	return (pvi = vi_netctrl_Connect(0,remIP,modPortTCP,terPortTCP,terPortUDP));
}

inline int CVINetModuleDll::Test()
{
	if (!IsOk()) return 0;

	return vi_netctrl_Test(pvi);
}

inline int	CVINetModuleDll::Disconnect(const char *remIP)
{
	if (!IsOk()) return 0;

	if(!pvi)
		return -1;

	int ret = vi_netctrl_Disconnect(pvi,remIP);
	if(!remIP)
		pvi = 0;
	return ret;
}

inline int	CVINetModuleDll::GetState(const char *remIP, double *pState, double *pMax, DWORD *pVer)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetState(pvi,remIP,pState,pMax,pVer);
}
inline int	CVINetModuleDll::GetVar(const char *remIP, int idVar, VI_VAR *pVar, DWORD *pVer)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetVar(pvi,remIP,idVar,pVar,pVer);
}
inline int	CVINetModuleDll::GetStr(const char *remIP, int idVar, char *pVar, int nMax, DWORD *pVer)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetStr(pvi,remIP,idVar,pVar,nMax,pVer);
}
inline int	CVINetModuleDll::PutVar(const char *remIP, const int *idVar, const VI_VAR *pVar, int cntVar)
{
	if (!IsOk()) return 0;
	return vi_netctrl_PutVar(pvi,remIP,idVar,pVar,cntVar);
}
inline int	CVINetModuleDll::PutStr(const char *remIP, const int *idVar, const char **pVar, int cntVar)
{
	if (!IsOk()) return 0;
	return vi_netctrl_PutStr(pvi,remIP,idVar,pVar,cntVar);
}
inline void CVINetModuleDll::SetExtVer(int ver)
{
	if (!IsOk()) return;
	vi_netctrl_SetExtVer(pvi,ver);
}
inline int CVINetModuleDll::GetExtVer()
{
	if (!IsOk()) return 0;

	return vi_netctrl_GetExtVer(pvi);
}
inline int	CVINetModuleDll::GetVarInfo(int idVar, char *name, int nMax, int *pType, VI_VAR *vMin, VI_VAR *vMax)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetVarInfo(idVar,name,nMax,pType,vMin,vMax);
}
inline VI_MODULE CVINetModuleDll::TermConnect(const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP)
{
	if (!IsOk()) return 0;
	if(pvi)
		return vi_netctrl_TermConnect(pvi,remIP,modPortTCP,terPortTCP,terPortUDP);
	return (pvi = vi_netctrl_TermConnect(0,remIP,modPortTCP,terPortTCP,terPortUDP));
}
inline HGLOBAL	CVINetModuleDll::GetImage(const char *remIP, int *pW, int *pH, int *pBPP,DWORD *pVer)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetImage(pvi,remIP, pW, pH, pBPP,pVer);
}
inline HGLOBAL	CVINetModuleDll::GetImagePack(const char *remIP, DWORD *pVer)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetImagePack(pvi,remIP,pVer);
}

inline int	CVINetModuleDll::SetCallbackOnVar(void *pFn, void *UserData)
{
	if (!IsOk()) return 0;
	return vi_netctrl_SetCallbackOnVar(pvi,pFn,UserData);
}

inline int	CVINetModuleDll::SetCallbackOnRqst(void *pFn, void *UserData)
{
	if (!IsOk()) return 0;
	return vi_netctrl_SetCallbackOnRqst(pvi,pFn,UserData);
}

inline int	CVINetModuleDll::SetCallbackOnStr(void *pFn, void *UserData)
{
	if (!IsOk()) return 0;
	return vi_netctrl_SetCallbackOnStr(pvi,pFn,UserData);
}

inline int	CVINetModuleDll::SetCallbackOnImage(void *pFn, void *UserData)
{
	if (!IsOk()) return 0;
	return vi_netctrl_SetCallbackOnImage(pvi,pFn,UserData);
}

inline int	CVINetModuleDll::SetCallbackOnImagePack(void *pFn, void *UserData)
{
	if (!IsOk()) return 0;
	return vi_netctrl_SetCallbackOnImagePack(pvi,pFn,UserData);
}

inline int	CVINetModuleDll::PutImage(void *pImg, int size,void *pAdd,int sizeAdd)
{
	if (!IsOk()) return 0;
	return vi_netctrl_PutImage(pvi,pImg,size,pAdd,sizeAdd);
}

inline int	CVINetModuleDll::GetTimes(ULONGLONG*ptRcv, ULONGLONG*ptSnd)
{
	if (!IsOk()) return 0;
	return vi_netctrl_GetTimes(pvi,ptRcv,ptSnd);
}

inline int	CVINetModuleDll::Config(const char *remIP, HWND hWndParent)
{
	if (!IsOk()) return 0;
	return  vi_netctrl_Config(pvi,remIP,hWndParent);
}

inline int CVINetModuleDll::AddTag(int id,const char *tag)
{
	if (!IsOk()) return 0;
	return vi_netctrl_AddTag(pvi,id,tag);
}

inline int CVINetModuleDll::RqstTag(const char *tag,int mode)
{
	if (!IsOk()) return 0;
	return vi_netctrl_RqstTag(pvi,tag,mode);
}

inline int CVINetModuleDll::Tag2Id(const char *tag)
{
	if (!IsOk()) return 0;
	return vi_netctrl_Tag2Id(pvi,tag);
}

inline const char* CVINetModuleDll::Id2Tag(int id)
{
	if (!IsOk()) return 0;
	return vi_netctrl_Id2Tag(pvi,id);
}
#endif
