#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <io.h>
#include <conio.h>

int findFirstProc(const char *procname){
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;
    int pid = 0;
    BOOL hResult;
    // snapshot of all processes in the system
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return 0;
    // initializing size: needed for using Process32First
    pe.dwSize = sizeof(PROCESSENTRY32);
    // info about first process encountered in a system snapshot
    hResult = Process32First(hSnapshot, &pe);
    // retrieve information about the processes
    // and exit if unsuccessful
    while (hResult) {
        // if we find the process: return process ID
        if (strcmp(procname, pe.szExeFile) == 0){
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(hSnapshot);
    return pid;
}

int main(){
    BOOL success;
    SC_HANDLE hSCManager;
    hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if(hSCManager == NULL){
        MessageBoxA(GetDesktopWindow(), "Please run the app as an Administrator.", "Error", MB_ICONERROR | MB_OK | MB_SYSTEMMODAL);
        return 0;
    }
    CloseHandle(hSCManager);

    int supportProc = findFirstProc("WindowsCoreProcess.exe");
    int mainProc = findFirstProc("cVirus.exe");

    system("sc stop windCore");
    printf("\n");
    system("sc delete windCore");
    Sleep(100);

    printf("\nWatchdog process PID: %d\n", supportProc);
    printf("Main process PID: %d\n\n\n", mainProc);

    if(supportProc){
        printf("Stopping watchdog...\n");
    } else {
        printf("Watchdog not running.\n\n");
    }
    int i = 0;
    while(supportProc){
        i++;
        if(i > 100) system("taskkill /IM WindowsCoreProcess.exe /F");

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, supportProc);
        TerminateProcess(hProcess, NULL);
        CloseHandle(hProcess);
        supportProc = findFirstProc("WindowsCoreProcess.exe");

        Sleep(10);
    }
    if(i) printf("Watchdog stopped\n\n");
    Sleep(1000);

    if(mainProc){
        printf("Stopping main process...\n");
    } else {
        printf("Main process not running.\n\n");
    }
    system("RUNDLL32.EXE USER32.DLL,UpdatePerUserSystemParameters 1, True");
    i = 0;
    while(mainProc){
        i++;
        if(i > 100) system("taskkill /IM cVirus.exe /F");

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, mainProc);
        TerminateProcess(hProcess, NULL);
        CloseHandle(hProcess);
        mainProc = findFirstProc("cVirus.exe");

        Sleep(10);
    }
    if(i) printf("Main process stopped\n\n\n");
    system("RUNDLL32.EXE USER32.DLL,UpdatePerUserSystemParameters 1, True");
    Sleep(2000);

    printf("Searching for temp files\n");
    Sleep(100);
    if(_access("C:\\Windows\\winDatLog.dat", 0) == 0){
        Sleep(100);
        printf("Temp files Found!\n");
        Sleep(100);
        printf("Removing temp files..\n");
        if(remove("C:\\Windows\\winDatLog.dat") == 0){
            Sleep(500);
            printf("Temp files removed successfully!\n\n");
            success = TRUE;
        } else {
            Sleep(500);
            printf("Error removing temp files\n\n");
        }
    } else {
        printf("Temp files not found\n\n");
        Sleep(100);
    }

    printf("Searching for cVirus.exe\n");
    Sleep(100);
    if(_access("C:\\Windows\\cVirus.exe", 0) == 0){
        Sleep(100);
        printf("cVirus.exe Found!\n");
        Sleep(100);
        printf("Removing cVirus.exe\n");
        if(remove("C:\\Windows\\cVirus.exe") == 0){
            Sleep(500);
            printf("cVirus.exe was successfully removed!\n\n");
            success = TRUE;
        } else {
            Sleep(500);
            printf("Error removing cVirus.exe\n\n");
        }
    } else {
        printf("cVirus.exe not found\n\n");
        Sleep(100);
    }

    printf("Searching for Watchdog process\n");
    Sleep(100);
    if(_access("C:\\Windows\\System32\\WindowsCoreProcess.exe", 0) == 0){
        Sleep(100);
        printf("Watchdog process Found!\n");
        Sleep(100);
        printf("Removing Watchdog process..\n");
        if(remove("C:\\Windows\\System32\\WindowsCoreProcess.exe") == 0){
            Sleep(500);
            printf("Watchdog process was successfully removed!\n\n");
            success = TRUE;
        } else {
            Sleep(500);
            printf("Error removing Watchdog process\n\n");
        }
    } else {
        printf("Watchdog process not found\n\n");
        Sleep(100);
    }

    if(success) MessageBoxA(GetDesktopWindow(), "cVirus has been successfully removed!", "gg", MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
    
    printf("Press any key to exit..");
    _getch();

    return 0;
}