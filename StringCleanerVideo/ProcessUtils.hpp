#ifndef PROCESSUTILS_HPP
#define PROCESSUTILS_HPP

#include <windows.h>
#include <string>
#include <vector>

class ProcessUtils {
public:
    static DWORD GetProcessIdFromExecutableName(const std::wstring& executableName);
    static DWORD GetProcessIdFromServiceName(const std::wstring& serviceName);
    static BOOL EnablePrivilege();
};

#endif