#include "StringScanner.hpp"
#include <algorithm>

std::vector<LPVOID> StringScanner::FindStringAddressesByOrder(DWORD processId, const std::string& sequence) {
    std::vector<LPVOID> addresses;
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (processHandle == NULL) {
        return addresses;
    }

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    LPVOID baseAddress = systemInfo.lpMinimumApplicationAddress;
    LPVOID maxAddress = systemInfo.lpMaximumApplicationAddress;

    std::string lowercaseSequence = sequence;
    std::transform(lowercaseSequence.begin(), lowercaseSequence.end(), lowercaseSequence.begin(), ::tolower);

    while (baseAddress < maxAddress) {
        MEMORY_BASIC_INFORMATION memoryInfo;
        if (VirtualQueryEx(processHandle, baseAddress, &memoryInfo, sizeof(memoryInfo)) == 0) {
            break;
        }

        if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && memoryInfo.Protect != PAGE_GUARD) {
            const size_t bufferSize = memoryInfo.RegionSize;
            std::vector<char> buffer(bufferSize);
            SIZE_T bytesRead;

            if (ReadProcessMemory(processHandle, memoryInfo.BaseAddress, &buffer[0], bufferSize, &bytesRead)) {
                std::string memoryString(buffer.begin(), buffer.end());
                std::transform(memoryString.begin(), memoryString.end(), memoryString.begin(), ::tolower);

                size_t foundIndex = memoryString.find(lowercaseSequence);
                while (foundIndex != std::string::npos) {
                    LPVOID stringAddress = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(memoryInfo.BaseAddress) + foundIndex);
                    addresses.push_back(stringAddress);
                    foundIndex = memoryString.find(lowercaseSequence, foundIndex + 1);
                }
            }
        }

        baseAddress = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(baseAddress) + memoryInfo.RegionSize);
    }

    CloseHandle(processHandle);
    return addresses;
}

std::vector<LPVOID> StringScanner::FindWStringAddressesByOrder(DWORD processId, const std::wstring& sequence) {
    std::vector<LPVOID> addresses;
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (processHandle == NULL) {
        return addresses;
    }

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    LPVOID baseAddress = systemInfo.lpMinimumApplicationAddress;
    LPVOID maxAddress = systemInfo.lpMaximumApplicationAddress;

    std::wstring lowercaseSequence = sequence;
    std::transform(lowercaseSequence.begin(), lowercaseSequence.end(), lowercaseSequence.begin(), ::towlower);

    while (baseAddress < maxAddress) {
        MEMORY_BASIC_INFORMATION memoryInfo;
        if (VirtualQueryEx(processHandle, baseAddress, &memoryInfo, sizeof(memoryInfo)) == 0) {
            break;
        }

        if (memoryInfo.State == MEM_COMMIT && memoryInfo.Protect != PAGE_NOACCESS && memoryInfo.Protect != PAGE_GUARD) {
            const size_t bufferSize = memoryInfo.RegionSize;
            std::vector<wchar_t> buffer(bufferSize / sizeof(wchar_t));
            SIZE_T bytesRead;

            if (ReadProcessMemory(processHandle, memoryInfo.BaseAddress, &buffer[0], bufferSize, &bytesRead)) {
                std::wstring memoryString(buffer.begin(), buffer.end());
                std::transform(memoryString.begin(), memoryString.end(), memoryString.begin(), ::towlower);

                size_t foundIndex = memoryString.find(lowercaseSequence);
                while (foundIndex != std::wstring::npos) {
                    LPVOID stringAddress = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(memoryInfo.BaseAddress) + (foundIndex * sizeof(wchar_t)));
                    addresses.push_back(stringAddress);
                    foundIndex = memoryString.find(lowercaseSequence, foundIndex + 1);
                }
            }
        }

        baseAddress = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(baseAddress) + memoryInfo.RegionSize);
    }

    CloseHandle(processHandle);
    return addresses;
}

std::wstring StringScanner::StringToWideString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}