#include "StringCleaner.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

void ClearScreen() {
    system(CLEAR_SCREEN);
}

void ClearCinBuffer() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

void AddProcessCleanup() {
    std::wstring processName;
    std::vector<std::string> strings;

    std::wcout << L"Process name (ex: lsass.exe): ";
    std::wcin >> processName;
    ClearCinBuffer();

    std::cout << "How many strings to clean? ";
    int stringCount;
    std::cin >> stringCount;
    ClearCinBuffer();

    for (int i = 0; i < stringCount; i++) {
        std::string str;
        std::cout << "String " << (i + 1) << ": ";
        std::getline(std::cin, str);
        strings.push_back(str);
    }

    StringCleaner::AddProcessToClean(processName, strings);
    std::cout << "\nProcess added for cleanup.\n";

    std::cout << "\nPress Enter to continue...";
    ClearCinBuffer();
    ClearScreen();
}

void AddServiceCleanup() {
    std::wstring serviceName;
    std::vector<std::string> strings;

    std::wcout << L"Service name (ex: Dnscache): ";
    std::wcin >> serviceName;
    ClearCinBuffer();

    std::cout << "How many strings to clean? ";
    int stringCount;
    std::cin >> stringCount;
    ClearCinBuffer();

    for (int i = 0; i < stringCount; i++) {
        std::string str;
        std::cout << "String " << (i + 1) << ": ";
        std::getline(std::cin, str);
        strings.push_back(str);
    }

    StringCleaner::AddServiceToClean(serviceName, strings);
    std::cout << "\nService added for cleanup.\n";

    std::cout << "\nPress Enter to continue...";
    ClearCinBuffer();
    ClearScreen();
}

int main() {
    int choice;

    ClearScreen();

    do {
        std::cout << "\n=== Memory String Cleaner ===\n";
        std::cout << "1. Add process to clean\n";
        std::cout << "2. Add service to clean\n";
        std::cout << "3. Execute cleanup\n";
        std::cout << "4. Clear target list\n";
        std::cout << "5. Exit\n";
        std::cout << "Choice: ";

        std::cin >> choice;
        ClearCinBuffer();

        switch (choice) {
        case 1:
            ClearScreen();
            AddProcessCleanup();
            break;
        case 2:
            ClearScreen();
            AddServiceCleanup();
            break;
        case 3:
            ClearScreen();
            StringCleaner::CleanStrings();
            std::cout << "Cleanup executed successfully.\n";
            std::cout << "\nPress Enter to continue...";
            ClearCinBuffer();
            ClearScreen();
            break;
        case 4:
            ClearScreen();
            StringCleaner::ClearAllTargets();
            std::cout << "Target list cleared.\n";
            std::cout << "\nPress Enter to continue...";
            ClearCinBuffer();
            ClearScreen();
            break;
        case 5:
            ClearScreen();
            std::cout << "Exiting...\n";
            break;
        default:
            ClearScreen();
            std::cout << "Invalid option!\n";
            std::cout << "\nPress Enter to continue...";
            ClearCinBuffer();
            ClearScreen();
        }

    } while (choice != 5);

    return 0;
}