#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: ParticlesGodInjector.exe <PID> <DLL>\n");
        return 1;
    }

    DWORD pid = atoi(argv[1]);
    const char* dllPath = argv[2];

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
    {
        printf("[ParticlesGodInjector] OpenProcess failed\n");
        return 1;
    }

    LPVOID mem = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1,
                                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!mem)
    {
        printf("[ParticlesGodInjector] VirtualAllocEx failed\n");
        return 1;
    }

    WriteProcessMemory(hProcess, mem, dllPath, strlen(dllPath) + 1, NULL);

    HMODULE hKernel = GetModuleHandleA("kernel32.dll");
    FARPROC loadLib = GetProcAddress(hKernel, "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                                        (LPTHREAD_START_ROUTINE)loadLib, mem, 0, NULL);
    if (!hThread)
    {
        printf("[ParticlesGodInjector] CreateRemoteThread failed\n");
        return 1;
    }

    WaitForSingleObject(hThread, 3000);
    printf("[ParticlesGodInjector] Injected successfully\n");

    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}