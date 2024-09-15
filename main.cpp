#include <windows.h>
#include <iostream>
#include <string>
#include <Tlhelp32.h>
#include <cstring>


void ToggleWifi(bool enable) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string command = "netsh interface set interface \"Wi-Fi\" ";
    command += enable ? "enable" : "disable";

    if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

DWORD FindProcessId(const char *filename) {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                CloseHandle(hProcess);
                return (DWORD) pEntry.th32ProcessID;
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    return 0;
}

bool killProcess(const char *filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                
                CloseHandle(hProcess);
                return 0;
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    return 1;
}

int main() {
    ToggleWifi(false);
  
    bool rpcnetRet = killProcess("rpcnet.exe");
    bool rpcnetpRet = killProcess("rpcnetp.exe");

    DWORD rpcnetProcessID = FindProcessId("rpcnet.exe");
    DWORD rpcnetpProcessID = FindProcessId("rpcnetp.exe");

    if (rpcnetProcessID != 0 || rpcnetpProcessID != 0) {
        MessageBox(NULL, "Failed to kill rpcnet.exe and rpcnetp.exe. Please stop the computer or kill these executables in C:\\Windows\\System32", "Error", MB_ICONERROR);
        return 1;
    }

    ToggleWifi(true);
    return 0;
}
