#ifndef STRINGCLEANER_HPP
#define STRINGCLEANER_HPP

#include <windows.h>
#include <string>
#include <vector>
#include <map>

class StringCleaner {
private:
    static bool cleaned;

public:
    static bool RemoveStringFromProcess(DWORD processId, const std::string& sequence, HANDLE processHandle);
    static int CleanStrings();
    static void AddProcessToClean(const std::wstring& processName, const std::vector<std::string>& strings);
    static void AddServiceToClean(const std::wstring& serviceName, const std::vector<std::string>& strings);
    static void ClearAllTargets();

private:
    static std::map<std::wstring, std::vector<std::string>> processSequences;
    static std::map<std::wstring, std::vector<std::string>> serviceSequences;
};

#endif