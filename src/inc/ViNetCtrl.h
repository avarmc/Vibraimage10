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
Создание нового подключения или добавление IP к текущему
/////////////////////////////////////////////////////////
 VI_MODULE vi_netctrl_Connect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
/////////////////////////////////////////////////////////
module:			указывает модуль, к  которому производить подключение. Если module=NULL, то создается новый.
remIP:			адрес модуля. 
modPortTCP:		сетевой порт модуля
terPortTCP:		локальный TCP порт
terPortUDP:		локальный UDP порт
/////////////////////////////////////////////////////////
функция возврацщает заголовок модуля или NULL в случае ошибки
*! terPortTCP,terPortUDP,modPortTCP указываются только при создании модуля (module=0). При добавлении IP к модулю оди долдны равняться 0.
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_AddTag
/////////////////////////////////////////////////////////
Создание метки для приема/передачи параметра
функция должна быть вызвана для каждого из параметров после вызова vi_netctrl_Connect
/////////////////////////////////////////////////////////
int vi_netctrl_AddTag(VI_MODULE module,int id,const char *tag)
/////////////////////////////////////////////////////////
module:			указывает модуль, к  которому производить подключение
id:				числовой идентификатор параметра в программе. (произвольное значение)
tag:			текстовый идентификатор параметра в Vibraimage10
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_RqstTag
/////////////////////////////////////////////////////////
Запрос к модулю на отправку выбранного параметра
функция должна быть вызвана для каждого из параметров после вызова vi_netctrl_Connect
/////////////////////////////////////////////////////////
int vi_netctrl_RqstTag(VI_MODULE module,const char *tag,int mode)
/////////////////////////////////////////////////////////
module:			указывает модуль, к  которому производить подключение
tag:			текстовый идентификатор параметра в Vibraimage10
mode:			режим отправки
					0 - в момент изменения
					1 - после обработки очередного кадра
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Test
/////////////////////////////////////////////////////////
Проверка возможности соединения
/////////////////////////////////////////////////////////
 int vi_netctrl_Test(VI_MODULE module);
/////////////////////////////////////////////////////////
module:			текущий модуль
/////////////////////////////////////////////////////////
функция производит тестовые соединения с (модулями/терминадом) 
возврацщает кодичество доступных соединений
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Disconnect
/////////////////////////////////////////////////////////
Завершение соединения
/////////////////////////////////////////////////////////
int	vi_netctrl_Disconnect(VI_MODULE module, const char *remIP);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			из списка будет удален соответствующий адрес. Если remIP=NULL модуль полностью завершает работу.
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetState
/////////////////////////////////////////////////////////
интегральная и критическая оценка опасности
/////////////////////////////////////////////////////////
int	vi_netctrl_GetState(VI_MODULE module, const char *remIP, double *pState, double *pMax, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL будет получено последнее принятое значение с любого адреса.
pState:			интегральная и оценка опасности (может быть 0)
pMax:			критический уровень опасности (может быть 0)
pVer:			адрес по которому будет сохранено время получения. (соответствует ((ULONG)GetTickCount64())). (может быть 0)
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
для успешной работы должны быть заданы метки для
TAG_VI_NETCTRL_VAR_STATE и TAG_VI_NETCTRL_VAR_STATE_CRITICAL
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetVar
/////////////////////////////////////////////////////////
получение параметра
/////////////////////////////////////////////////////////
int	vi_netctrl_GetVar(VI_MODULE module, const char *remIP, int idVar, VI_VAR *pVar, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL будет получено последнее принятое значение с любого адреса.
idVar:			номер запрашиваемого параметра
pVar:			адрес по которому будет сохранено значение. (может быть 0)
pVer:			адрес по которому будет сохранено время получения. (соответствует ((ULONG)GetTickCount64())). (может быть 0)
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetStr [зарезервировано. в данной версии не используется]
/////////////////////////////////////////////////////////
получение строкового параметра
/////////////////////////////////////////////////////////
int	vi_netctrl_GetStr(VI_MODULE module, const char *remIP, int idVar, char *pVar, int nMax, DWORD *pVer);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL будет получено последнее принятое значение с любого адреса.
idVar:			номер запрашиваемого параметра
pVar:			адрес по которому будет сохранено значение. (может быть 0)
nMax:			максимальное число символов в pVar
pVer:			адрес по которому будет сохранено время получения. (соответствует ((ULONG)GetTickCount64())). (может быть 0)
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_PutVar
/////////////////////////////////////////////////////////
передача параметра
/////////////////////////////////////////////////////////
int	vi_netctrl_PutVar(VI_MODULE module, const char *remIP, const int *idVar, const VI_VAR *pVar, int cntVar);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL параметр будет отправлен по всем известным адресам.
idVar:			массив, размером [cntVar], содержащий номера параметров
pVar:			массив, размером [cntVar], содержащий значения параметров
cntVer:			количество параметров
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_PutStr [зарезервировано. в данной версии не используется]
/////////////////////////////////////////////////////////
передача строкового параметра
/////////////////////////////////////////////////////////
int	vi_netctrl_PutStr(VI_MODULE module, const char *remIP, const int *idVar, const char **pVar, int cntVar);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL параметр будет отправлен по всем известным адресам.
idVar:			массив, размером [cntVar], содержащий номера параметров
pVar:			массив, размером [cntVar], содержащий адреса строк
cntVer:			количество параметров
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetVarInfo [в данной версии реализовано частично]
/////////////////////////////////////////////////////////
получение информации о параметре
/////////////////////////////////////////////////////////
int	vi_netctrl_GetVarInfo(int idVar, char *name, int nMax, int *pType, VI_VAR *vMin, VI_VAR *vMax);
/////////////////////////////////////////////////////////
idVar:			номер запрашиваемого параметра
name:			адрес по которому будет сохранено название параметра [nMax]
nMax:			максимальное число символов в name
pType:			определяет тип значения (VI_VAR_SUB); -1 если не определено
vMin:			минимальное значение параметра (ориентировочно)
vMax:			максимальное значение параметра (ориентировочно)
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetImage
/////////////////////////////////////////////////////////
получение изображение
/////////////////////////////////////////////////////////
HGLOBAL	vi_netctrl_GetImage(VI_MODULE module, const char *remIP, int *pW, int *pH, int *pBPP,DWORD *pVer);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL будет получено последнее принятое значение с любого адреса.
pW:				ширина изображения (может быть 0)
pH:				высота изображения (может быть 0)
pBPP:			[размер точки] изображения (может быть 0)
pVer:			адрес по которому будет сохранено время получения. (соответствует ((ULONG)GetTickCount64())). (может быть 0)
/////////////////////////////////////////////////////////
функция возврацщает HGLOBAL в случае успешного завершения и 0 в случае ошибки
для получения адреса следует использовать функции GlobalLock, GlobalUnlock
после завершения работы с кадром следует использовать GlobalFree
/////////////////////////////////////////////////////////*/


/*/////////////////////////////////////////////////////////
 vi_netctrl_GetImagePack
/////////////////////////////////////////////////////////
получение изображение в упакованном виде (как поступило с модуля)
/////////////////////////////////////////////////////////
HGLOBAL	vi_netctrl_GetImagePack(VI_MODULE module, const char *remIP,DWORD *pVer);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL будет получено последнее принятое значение с любого адреса.
pVer:			адрес по которому будет сохранено время получения. (соответствует ((ULONG)GetTickCount64())). (может быть 0)
/////////////////////////////////////////////////////////
функция возврацщает HGLOBAL в случае успешного завершения и 0 в случае ошибки
для получения адреса следует использовать функции GlobalLock, GlobalUnlock
после завершения работы с кадром следует использовать GlobalFree
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_Config
/////////////////////////////////////////////////////////
Вызов окна настроек [зарезервировано. в данной версии не реализовано]
/////////////////////////////////////////////////////////
int	vi_netctrl_Config(VI_MODULE module, const char *remIP, HWND hWndParent);
/////////////////////////////////////////////////////////
module:			текущий модуль
remIP:			адрес модуля. Если remIP=NULL настройки бубут переданы всем модулям
hWndParent:		окно приложения
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnVar
/////////////////////////////////////////////////////////
установка обработчика на прием параметров
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnVar(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			текущий модуль
pFn:			static void CallbackOnVar(void *UserData,VI_MODULE module,const char *ip,const int *idVar, const VI_VAR *pVar, int cntVar);
UserData:		данные пользователя
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnStr
/////////////////////////////////////////////////////////
установка обработчика на прием строковых параметров
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnStr(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			текущий модуль
pFn:			static void CallbackOnStr(void *UserData,VI_MODULE module,const char *ip,const int *idVar, const char **pVar, int cntVar);
UserData:		данные пользователя
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnImage
/////////////////////////////////////////////////////////
установка обработчика на прием изображений
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnImage(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			текущий модуль
pFn:			static void CallbackOnImage(void *UserData,VI_MODULE module,const char *ip,void *pBits, int w, int h, int bpp);
UserData:		данные пользователя
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnImagePack
/////////////////////////////////////////////////////////
установка обработчика на прием изображений в упакованном виде
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnImagePack(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			текущий модуль
pFn:			static void CallbackOnImagePack(void *UserData,VI_MODULE module,const char *ip,void *ptr,int size,void *pData,int dataSize);
UserData:		данные пользователя
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_GetTimes
/////////////////////////////////////////////////////////
получение времени последнего обращения к сети ( см. ((ULONG)GetTickCount64()) )
/////////////////////////////////////////////////////////
int	vi_netctrl_GetTimes(VI_MODULE module, ULONGLONG *ptRcv,ULONGLONG *ptSnd)
/////////////////////////////////////////////////////////
module:			текущий модуль
ptRcv:			время приема; может быть 0
ptSend:			время отправки; может быть 0
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
Функции модуля ЭЛСИС (используются только ПО модуля)
/////////////////////////////////////////////////////////
VI_MODULE vi_netctrl_TermConnect(VI_MODULE module, const char *remIP, int modPortTCP, int terPortTCP, int terPortUDP);
int	vi_netctrl_PutImage(VI_MODULE module, void *pImg, int size,void *pAdd,int sizeAdd);
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
 vi_netctrl_SetCallbackOnRqst
/////////////////////////////////////////////////////////
установка обработчика на прием запросов на отсылку
/////////////////////////////////////////////////////////
int	vi_netctrl_SetCallbackOnRqst(VI_MODULE module, void *pFn, void *UserData);
/////////////////////////////////////////////////////////
module:			текущий модуль
pFn:			static void CallbackOnVar(void *UserData,VI_MODULE module,const char *ip,const char* tag,int mode);
UserData:		данные пользователя
/////////////////////////////////////////////////////////
функция возврацщает 0 в случае успешного завершения
/////////////////////////////////////////////////////////*/

/*/////////////////////////////////////////////////////////
	CVINetModuleDll функции DLL, собранные в класс
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
