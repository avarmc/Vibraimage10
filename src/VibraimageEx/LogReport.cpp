#include "stdafx.h"
#include "LogReport.h"
#include <stdio.h>
#include "Event_log.h"



CLogReport::CLogReport() {

}

CLogReport::~CLogReport() {

}


bool CLogReport::Register(CString a_name) {
    bool ret = true;
    a_name.Replace('\\', '_');

    CString key_path(L"SYSTEM\\CurrentControlSet\\Services\\"
        L"EventLog\\Application\\" + a_name);

    HKEY key;

    DWORD last_error = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
        key_path,
        0,
        0,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        0,
        &key,
        0);

    if (ERROR_SUCCESS == last_error)
    {
        TCHAR exe_path[MAX_PATH + 1] = { 0 };
        GetModuleFileName(NULL, exe_path, MAX_PATH + 1);

        DWORD last_error;
        const DWORD types_supported = EVENTLOG_ERROR_TYPE |
            EVENTLOG_WARNING_TYPE |
            EVENTLOG_INFORMATION_TYPE;

        last_error = RegSetValueEx(key,
            L"EventMessageFile",
            0,
            REG_SZ,
            (BYTE *)exe_path,
            (DWORD)(sizeof(TCHAR)* (wcslen(exe_path)+1)));

        if (ERROR_SUCCESS == last_error)
        {
            last_error = RegSetValueEx(key,
                L"TypesSupported",
                0,
                REG_DWORD,
                (LPBYTE)&types_supported,
                sizeof(types_supported));
        }

        if (ERROR_SUCCESS != last_error)
        {
            ret = false;
        }

        RegCloseKey(key);
    }
    else
    {
        ret = false;
    }



    return ret;
}

void CLogReport::Unregister(CString a_name) {

    a_name.Replace('\\', '_');

    CString key_path(L"SYSTEM\\CurrentControlSet\\Services\\"
        L"EventLog\\Application\\" + a_name);

    DWORD last_error = RegDeleteKey(HKEY_LOCAL_MACHINE,
        key_path);

    if (ERROR_SUCCESS != last_error)
    {
       
    }
}

bool CLogReport::IsRegistered(CString a_name) {
    a_name.Replace('\\', '_');

    CString key_path(L"SYSTEM\\CurrentControlSet\\Services\\"
        L"EventLog\\Application\\" + a_name);

    HKEY key;

    DWORD last_error = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_path,0, KEY_READ,&key);

    if (ERROR_SUCCESS != last_error)
        return false;
    RegCloseKey(key);
    return true;
}

bool CLogReport::LogMessage(CString a_msg, const WORD a_type, CString a_name) {
    DWORD event_id;
    a_name.Replace('\\', '_');

    if (!IsRegistered(a_name)) {
        if (!Register(a_name))
            return false;
    }

    switch (a_type)
    {
    case EVENTLOG_ERROR_TYPE:
        event_id = MSG_ERROR_1;
        break;
    case EVENTLOG_WARNING_TYPE:
        event_id = MSG_WARNING_1;
        break;
    case EVENTLOG_INFORMATION_TYPE:
        event_id = MSG_INFO_1;
        break;
    default:
     //   std::cerr << "Unrecognised type: " << a_type << "\n";
        event_id = MSG_INFO_1;
        break;
    }

    HANDLE h_event_log = RegisterEventSource(0, a_name);

    if (0 == h_event_log)
    {
       // std::cerr << "Failed open source '" << a_name << "': " <<        GetLastError() << "\n";
        return false;
    }
    else
    {
        LPCTSTR message = a_msg;

        if (FALSE == ReportEvent(h_event_log,
            a_type,
            0,
            event_id,
            0,
            1,
            0,
            &message,
            0))
        {
            // std::cerr << "Failed to write message: " <<            GetLastError() << "\n";
            return false;
        }

        DeregisterEventSource(h_event_log);
    }

    return true;
}