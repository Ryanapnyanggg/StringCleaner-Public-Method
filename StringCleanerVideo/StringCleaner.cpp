#include "StringCleaner.hpp"
#include "StringScanner.hpp"
#include "ProcessUtils.hpp"
#include <iostream>

bool StringCleaner::cleaned = false;
std::map<std::wstring, std::vector<std::string>> StringCleaner::processSequences;
std::map<std::wstring, std::vector<std::string>> StringCleaner::serviceSequences;

bool StringCleaner::RemoveStringFromProcess(DWORD processId, const std::string& sequence, HANDLE processHandle) {
    std::vector<LPVOID> addresses = StringScanner::FindStringAddressesByOrder(processId, sequence);
    for (const auto& address : addresses) {
        SIZE_T sequenceSize = sequence.size();
        std::string emptySequence(sequenceSize, '\0');
        if (WriteProcessMemory(processHandle, address, emptySequence.c_str(), sequenceSize, NULL) != 0) {
            cleaned = true;
        }
    }

    std::wstring wsequence = StringScanner::StringToWideString(sequence);
    std::vector<LPVOID> waddresses = StringScanner::FindWStringAddressesByOrder(processId, wsequence);
    for (const auto& waddress : waddresses) {
        SIZE_T sequenceSize = wsequence.size() * sizeof(wchar_t);
        std::wstring emptyWSequence(sequenceSize / sizeof(wchar_t), L'\0');
        if (WriteProcessMemory(processHandle, waddress, emptyWSequence.c_str(), sequenceSize, NULL) != 0) {
            cleaned = true;
        }
    }

    return cleaned;
}

void StringCleaner::AddProcessToClean(const std::wstring& processName, const std::vector<std::string>& strings) {
    processSequences[processName] = strings;
}

void StringCleaner::AddServiceToClean(const std::wstring& serviceName, const std::vector<std::string>& strings) {
    serviceSequences[serviceName] = strings;
}

void StringCleaner::ClearAllTargets() {
    processSequences.clear();
    serviceSequences.clear();
}

int StringCleaner::CleanStrings() {
    ProcessUtils::EnablePrivilege();

    for (auto& entry : processSequences) {
        const std::wstring& processName = entry.first;
        const std::vector<std::string>& stringsToRemove = entry.second;

        DWORD pid = ProcessUtils::GetProcessIdFromExecutableName(processName);
        if (pid != 0) {
            HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
            if (processHandle != nullptr) {
                for (const std::string& sequence : stringsToRemove) {
                    RemoveStringFromProcess(pid, sequence, processHandle);
                }
                CloseHandle(processHandle);
            }
        }
    }

    for (auto& entry : serviceSequences) {
        const std::wstring& serviceName = entry.first;
        const std::vector<std::string>& stringsToRemove = entry.second;

        DWORD pid = ProcessUtils::GetProcessIdFromServiceName(serviceName);
        if (pid != 0) {
            HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
            if (processHandle != nullptr) {
                for (const std::string& sequence : stringsToRemove) {
                    RemoveStringFromProcess(pid, sequence, processHandle);
                }
                CloseHandle(processHandle);
            }
        }
    }

    return 0;
}