#include "ProcessUtils.hpp"
#include <tlhelp32.h>
#include <winsvc.h>

DWORD ProcessUtils::GetProcessIdFromExecutableName(const std::wstring& executableName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(snapshot, &processEntry)) {
        CloseHandle(snapshot);
        return 0;
    }

    DWORD pid = 0;
    do {
        std::wstring processName(processEntry.szExeFile);
        if (processName == executableName) {
            pid = processEntry.th32ProcessID;
            break;
        }
    } while (Process32NextW(snapshot, &processEntry));

    CloseHandle(snapshot);
    return pid;
}

DWORD ProcessUtils::GetProcessIdFromServiceName(const std::wstring& serviceName) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) {
        return 0;
    }

    int bufferSize = WideCharToMultiByte(CP_ACP, 0, serviceName.c_str(), -1, NULL, 0, NULL, NULL);
    char* serviceNameA = new char[bufferSize];
    WideCharToMultiByte(CP_ACP, 0, serviceName.c_str(), -1, serviceNameA, bufferSize, NULL, NULL);

    SC_HANDLE service = OpenServiceA(scm, serviceNameA, SERVICE_QUERY_STATUS);
    delete[] serviceNameA;

    if (!service) {
        CloseServiceHandle(scm);
        return 0;
    }

    SERVICE_STATUS_PROCESS status;
    DWORD bytesNeeded;
    if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&status), sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 0;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return status.dwProcessId;
}

BOOL ProcessUtils::EnablePrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tokenPrivileges;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return FALSE;

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        return FALSE;
    }

    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        CloseHandle(hToken);
        return FALSE;
    }

    CloseHandle(hToken);
    return TRUE;
}