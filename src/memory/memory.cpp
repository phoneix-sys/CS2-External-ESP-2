#include "memory.h"

uintptr_t mem::GetProcID(const wchar_t* processName) {
    HANDLE procHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);

    Process32First(procHandle, &process);
    do {
        if (!wcscmp(processName, process.szExeFile)) {
            CloseHandle(procHandle);
            procID = process.th32ProcessID;
            pHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
            return process.th32ProcessID;
        }
    } while (Process32Next(procHandle, &process));

    CloseHandle(procHandle);
    return 0;
}

uintptr_t mem::GetModuleBase(uintptr_t procID, const wchar_t* module_name) {
    HANDLE procHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    MODULEENTRY32 m;
    m.dwSize = sizeof(MODULEENTRY32);
    Module32First(procHandle, &m);

    do {
        if (!wcscmp(module_name, m.szModule)) {
            CloseHandle(procHandle);
            return (uintptr_t)m.modBaseAddr;
        }
    } while (Module32Next(procHandle, &m));
    CloseHandle(procHandle);
    return 0;
}