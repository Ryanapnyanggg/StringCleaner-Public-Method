#ifndef STRINGSCANNER_HPP
#define STRINGSCANNER_HPP

#include <windows.h>
#include <vector>
#include <string>

class StringScanner {
public:
    static std::vector<LPVOID> FindStringAddressesByOrder(DWORD processId, const std::string& sequence);
    static std::vector<LPVOID> FindWStringAddressesByOrder(DWORD processId, const std::wstring& sequence);
    static std::wstring StringToWideString(const std::string& str);
};

#endif