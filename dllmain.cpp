#include "stdafx.h"
#include "d3d11hook.h"
#include "Menu.h"
#include <GameHooks.h>

LPCSTR AppWindowTitle = "Grand Theft Auto V"; // Targeted D11 Application Window Title.
HWND hWindow = NULL;
Menu menu = Menu();
unique_ptr<job_pool> g_job_pool = make_unique<job_pool>();
PlayerList player_list = PlayerList();
ofstream log_file;
extern "C" uintptr_t sge_return_addy = NULL;

string to_upper(string& in)
{
    for_each(in.begin(), in.end(), [](char& c) {c = toupper(c); });
    return in;
}

MODULEENTRY32 GetRemoteModuleInfo(DWORD pid, char* module) {
    MODULEENTRY32 modEntry;
    HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    modEntry.dwSize = sizeof(MODULEENTRY32);
    Module32First(tlh, &modEntry);
    do {
        if (!_stricmp(modEntry.szModule, module)) {
        //wchar_t w_module;
        //MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, module, sizeof(module), &w_module, 0);
        //if (!_wcsicmp(modEntry.szModule, &w_module)) {
            return modEntry;
        }
    } while (Module32Next(tlh, &modEntry));

    return modEntry;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD wndPid = NULL;
    DWORD threadId = GetWindowThreadProcessId(hWnd, &wndPid);
    if (GetCurrentProcessId() == wndPid)
    {
        int len = GetWindowTextLength(hWnd);
        if (len < 15) return true;
        char* title = new char[len + 1];
        len = GetWindowTextA(hWnd, title, len);
        fmt::print("{} : {} - found window: {} ({})\n", GetCurrentProcessId(), wndPid, title, len);
        AppWindowTitle = reinterpret_cast<LPCSTR>(title);
        hWindow = hWnd;
        delete title;
        return FALSE;
    }

    return TRUE;
}

void get_window_name()
{
    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!EnumWindows(EnumWindowsProc, NULL))
    {
        /*char FailedToFindWindow[] = "Failed to find GTA window";
        HiddenText FailedToFindWindowEnc = FailedToFindWindow;
        FailedToFindWindowEnc.DecryptString();
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        cout << FailedToFindWindowEnc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return;*/
    }
}

void scan_for_patterns()
{
    //fmt::print("scan\n");
    const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    // Scan for lock mouse pointer address
    char scanning_for_m[] = "Scanning for M";
    HiddenText scanning_for_m_enc = scanning_for_m;
    scanning_for_m_enc.DecryptString();
    fmt::print("{}\n", scanning_for_m_enc.GetDecryptedString());

    char m_sig[] = "\x80\x3D\xA2\x22\x91\x02\x00";
    HiddenText m_sig_enc = m_sig;
    m_sig_enc.DecryptString();
    menu.m_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        m_sig_enc.GetDecryptedString(), "xxxxxxx", 7));
    if (reinterpret_cast<void*>(menu.m_addy) == nullptr)
    {
        char failed_to_find_m[] = "Failed to find M";
        HiddenText failed_to_find_m_enc = failed_to_find_m;
        failed_to_find_m_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_m_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.m_addy));

    // Scan for dlc global to return true for r* admin check
    char scanning_for_d[] = "Scanning for D";
    HiddenText scanning_for_d_enc = scanning_for_d;
    scanning_for_d_enc.DecryptString();
    fmt::print("{}\n", scanning_for_d_enc.GetDecryptedString());

    char d_sig[] = "\x8a\x05\xcc\xcc\xcc\xcc\xeb\xcc\x81\xf9";
    HiddenText d_sig_enc = d_sig;
    d_sig_enc.DecryptString();
    menu.d_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        d_sig_enc.GetDecryptedString(), "xx????x?xx", 10));
    if (reinterpret_cast<void*>(menu.d_addy) == nullptr)
    {
        char failed_to_find_d[] = "Failed to find D";
        HiddenText failed_to_find_d_enc = failed_to_find_d;
        failed_to_find_d_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_d_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.d_addy));
    
    // Scan for Player Join Event
    char scanning_for_pje[] = "Scanning for PJE";
    HiddenText scanning_for_pje_enc = scanning_for_pje;
    scanning_for_pje_enc.DecryptString();
    fmt::print("{}\n", scanning_for_pje_enc.GetDecryptedString());

    ///char pje_sig[] = "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x56\x48\x83\xEC\x20\x48\x8B\x59\x40\x4C\x8B\xF2\x48\x8B\xF1\x48\x85\xDB\x74\x22\x48\x8B\x02\x48\x8B\xCA\xFF\x50\x18\x4C\x8D\x05\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x4C\x8B\xC8\x48\x8B\xCB\xE8\x00\x00\x00\x00\x44\x0F\xB7\x46\x00\x33\xDB\x33\xC9\x49\x63\xD0\x45\x85\xC0\x7E\x18\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x0C\x48\xFF\xC1\x48\x83\xC0\x08\x48\x3B\xCA\x7C\xEC\x33\xFF\x33\xC9\x45\x85\xC0\x7E\x69\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x10\x48\xFF\xC1\xFF\xC7\x48\x83\xC0\x08\x49\x3B\xC8\x7C\xEA\xEB\x4D\x48\x8B\x4B\x08\x48\x85\xC9\x74\x12\x48\x8B\x49\x48\x48\x85\xC9\x74\x09\x48\x8B\x01\x49\x8B\xD6\xFF\x50\x10";
    //char pje_sig[] = "\x4C\x8B\xF2\x48\x8B\xF1\x48\x85\xDB\x74\x22\x48\x8B\x02\x48\x8B\xCA\xFF\x50\x18\x4C\x8D\x05\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x4C\x8B\xC8\x48\x8B\xCB\xE8\x00\x00\x00\x00\x44\x0F\xB7\x46\x00\x33\xDB\x33\xC9\x49\x63\xD0\x45\x85\xC0\x7E\x18\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x0C\x48\xFF\xC1\x48\x83\xC0\x08\x48\x3B\xCA\x7C\xEC\x33\xFF\x33\xC9\x45\x85\xC0\x7E\x69\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x10\x48\xFF\xC1\xFF\xC7\x48\x83\xC0\x08\x49\x3B\xC8\x7C\xEA\xEB\x4D\x48\x8B\x4B\x08\x48\x85\xC9\x74\x12\x48\x8B\x49\x48\x48\x85\xC9\x74\x09\x48\x8B\x01\x49\x8B\xD6\xFF\x50\x10";
    char pje_sig[] = "\x41\x56\x48\x83\xEC\x20\x48\x8B\x59\x40\x4C\x8B\xF2\x48\x8B\xF1\x48\x85\xDB\x74\x22\x48\x8B\x02\x48\x8B\xCA\xFF\x50\x18\x4C\x8D\x05\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x4C\x8B\xC8\x48\x8B\xCB\xE8\x00\x00\x00\x00\x44\x0F\xB7\x46\x00\x33\xDB\x33\xC9\x49\x63\xD0\x45\x85\xC0\x7E\x18\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x0C\x48\xFF\xC1\x48\x83\xC0\x08\x48\x3B\xCA\x7C\xEC\x33\xFF\x33\xC9\x45\x85\xC0\x7E\x69\x48\x8B\x46\x50\x48\x8B\x18\x48\x85\xDB\x75\x10\x48\xFF\xC1\xFF\xC7\x48\x83\xC0\x08\x49\x3B\xC8\x7C\xEA\xEB\x4D\x48\x8B\x4B\x08\x48\x85\xC9\x74\x12\x48\x8B\x49\x48\x48\x85\xC9\x74\x09\x48\x8B\x01\x49\x8B\xD6\xFF\x50\x10";
    HiddenText pje_sig_enc = pje_sig;
    pje_sig_enc.DecryptString();
    menu.pje_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            //pje_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx????xxxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 175));
            //pje_sig_enc.GetDecryptedString(), "xxxxxxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx????xxxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 150));
            pje_sig_enc.GetDecryptedString(), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxx????xxxxxxx????xxxx?xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 160));
    if (reinterpret_cast<void*>(menu.pje_addy) == nullptr)
    {
        char failed_to_find_pje[] = "Failed to find PJE";
        HiddenText failed_to_find_pje_enc = failed_to_find_pje;
        failed_to_find_pje_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_pje_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.pje_addy));

    // Scan for Player Leave Event
    char scanning_for_ple[] = "Scanning for PLE";
    HiddenText scanning_for_ple_enc = scanning_for_ple;
    scanning_for_ple_enc.DecryptString();
    fmt::print("{}\n", scanning_for_ple_enc.GetDecryptedString());

    //char ple_sig[] = "\xE8\x00\x00\x00\x00\x45\x33\xD2\x45\x8B\xC2\x44\x39\x93\x00\x00\x00\x00";
    char ple_sig[] = "\x41\x56\x48\x83\xec\xcc\x48\x8b\x59\xcc\x4c\x8b\xf2\x48\x8b\xf1\x48\x85\xdb\x74\xcc\x48\x8b\x02\x48\x8b\xca\xff\x50\xcc\x4c\x8d\x05\xcc\xcc\xcc\xcc\x48\x8d\x15\xcc\xcc\xcc\xcc\x4c\x8b\xc8\x48\x8b\xcb\xe8\xcc\xcc\xcc\xcc\x44\x0f\xb7\x46\xcc\x33\xdb\x33\xc9\x49\x63\xd0\x45\x85\xc0\x7e\xcc\x48\x8b\x46\xcc\x48\x8b\x18\x48\x85\xdb\x75\xcc\x48\xff\xc1\x48\x83\xc0\xcc\x48\x3b\xca\x7c\xcc\x33\xff\x33\xc9\x45\x85\xc0\x7e\xcc\x48\x8b\x46\xcc\x48\x8b\x18\x48\x85\xdb\x75\xcc\x48\xff\xc1\xff\xc7\x48\x83\xc0\xcc\x49\x3b\xc8\x7c\xcc\xeb\xcc\x48\x8b\x4b\xcc\x48\x85\xc9\x74\xcc\x48\x8b\x49\xcc\x48\x85\xc9\x74\xcc\x48\x8b\x01\x49\x8b\xd6\xff\x50\xcc\x48\x8b\x5b\xcc\x44\x0f\xb7\x46\xcc\x48\x63\xd7\x48\x85\xdb\x75\xcc\x48\xc1\xe2\xcc\x8d\x4f\xcc\x41\x3b\xc8\x7d\xcc\x48\x8b\x46\xcc\x48\x83\xc2\xcc\xff\xc7\x48\x8b\x1c\x02\xff\xc1\x48\x85\xdb\x74\xcc\x48\x85\xdb\x75\xcc\x48\x8b\x5c\x24\xcc\x48\x8b\x74\x24\xcc\x48\x8b\x7c\x24\xcc\x48\x83\xc4\xcc\x41\x5e\xc3\x8b\x41";
    HiddenText ple_sig_enc = ple_sig;
    ple_sig_enc.DecryptString();
    menu.ple_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            //ple_sig_enc.GetDecryptedString(), "x????xxxxxxxxx????", 18));
            ple_sig_enc.GetDecryptedString(), "xxxxx?xxx?xxxxxxxxxx?xxxxxxxx?xxx????xxx????xxxxxxx????xxxx?xxxxxxxxxxx?xxx?xxxxxxx?xxxxxx?xxxx?xxxxxxxx?xxx?xxxxxxx?xxxxxxxx?xxxx?x?xxx?xxxx?xxx?xxxx?xxxxxxxx?xxx?xxxx?xxxxxxx?xxx?xx?xxxx?xxx?xxx?xxxxxxxxxxxx?xxxx?xxxx?xxxx?xxxx?xxx?xxxxx", 239));
    if (reinterpret_cast<void*>(menu.ple_addy) == nullptr)
    {
        char failed_to_find_ple[] = "Failed to find PLE";
        HiddenText failed_to_find_ple_enc = failed_to_find_ple;
        failed_to_find_ple_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_ple_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.ple_addy));

    // Scan for Trigger Script Event
    char scanning_for_tse[] = "Scanning for TSE";
    HiddenText scanning_for_tse_enc = scanning_for_tse;
    scanning_for_tse_enc.DecryptString();
    fmt::print("{}\n", scanning_for_tse_enc.GetDecryptedString());

    char tse_sig[] = "\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x18\x48\x89\x78\x20\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x45\x8B\xF0\x41\x8B\xF9";
    HiddenText tse_sig_enc = tse_sig;
    tse_sig_enc.DecryptString();
    menu.tse_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            tse_sig_enc.GetDecryptedString(), "xxxxxxxxxxxxxxxxxxxxxxxx????xxxxxx", 34));
    if (reinterpret_cast<void*>(menu.tse_addy) == nullptr)
    {
        char failed_to_find_tse[] = "Failed to find TSE";
        HiddenText failed_to_find_tse_enc = failed_to_find_tse;
        failed_to_find_tse_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_tse_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.tse_addy));

    // Scan for datbitBuffer read dword function
    char scanning_for_rdw[] = "Scanning for RDW";
    HiddenText scanning_for_rdw_enc = scanning_for_rdw;
    scanning_for_rdw_enc.DecryptString();
    fmt::print("{}\n", scanning_for_rdw_enc.GetDecryptedString());

    char rdw_sig[] = "\x48\x89\x5c\x24\xcc\x48\x89\x74\x24\xcc\x57\x48\x83\xec\xcc\x48\x8b\xd9\x33\xc9\x41\x8b\xf0\x8a\x43";
    HiddenText rdw_sig_enc = rdw_sig;
    rdw_sig_enc.DecryptString();
    menu.rdw_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        rdw_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxxxxxxxx", 25));
    if (reinterpret_cast<void*>(menu.rdw_addy) == nullptr)
    {
        char failed_to_find_rdw[] = "Failed to find RDW";
        HiddenText failed_to_find_rdw_enc = failed_to_find_rdw;
        failed_to_find_rdw_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_rdw_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.rdw_addy));

    // Scan for datbitBuffer write int function
    char scanning_for_wint[] = "Scanning for WINT";
    HiddenText scanning_for_wint_enc = scanning_for_wint;
    scanning_for_wint_enc.DecryptString();
    fmt::print("{}\n", scanning_for_wint_enc.GetDecryptedString());

    char wint_sig[] = "\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x68\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x41\x56\x48\x83\xec\xcc\x8b\xea\xbf";
    HiddenText wint_sig_enc = wint_sig;
    wint_sig_enc.DecryptString();
    menu.wint_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        wint_sig_enc.GetDecryptedString(), "xxxxxx?xxx?xxx?xxx?xxxxx?xxx", 28));
    if (reinterpret_cast<void*>(menu.wint_addy) == nullptr)
    {
        char failed_to_find_wint[] = "Failed to find WINT";
        HiddenText failed_to_find_wint_enc = failed_to_find_wint;
        failed_to_find_wint_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_wint_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.wint_addy));

    // Scan for CScriptedGameEvent
    char scanning_for_sge[] = "Scanning for SGE";
    HiddenText scanning_for_sge_enc = scanning_for_sge;
    scanning_for_sge_enc.DecryptString();
    fmt::print("{}\n", scanning_for_sge_enc.GetDecryptedString());

    //char sge_sig[] = "\x40\x53\x48\x81\xEC\x00\x00\x00\x00\x44\x8B\x81\x00\x00\x00\x00";
    char sge_sig[] = "\x48\x89\x5c\x24\xcc\x48\x89\x6c\x24\xcc\x48\x89\x74\x24\xcc\x57\x48\x81\xec\xcc\xcc\xcc\xcc\x48\x8b\xe9\x48\x8d\x4c\x24\xcc\x41\x0f\xb7\xd9\x49\x8b\xf8\x48\x8b\xf2\xe8\xcc\xcc\xcc\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8d\x54\x24\xcc\x4c\x8b\xce\x48\x8b\xc8\x8b\x84\x24\xcc\xcc\xcc\xcc\x4c\x8b\xc5\x89\x44\x24\xcc\x8b\x84\x24\xcc\xcc\xcc\xcc\x89\x44\x24\xcc\x66\x89\x5c\x24\xcc\x48\x89\x7c\x24\xcc\xe8\xcc\xcc\xcc\xcc\x48\x8d\x05\xcc\xcc\xcc\xcc\x48\x8d\x4c\x24\xcc\x48\x89\x44\x24";
    HiddenText sge_sig_enc = sge_sig;
    sge_sig_enc.DecryptString();
    menu.sge_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        //sge_sig_enc.GetDecryptedString(), "xxxxx????xxx????", 16));
		sge_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxx????xxxxxxx?xxxxxxxxxxx????x????xxxx?xxxxxxxxx????xxxxxx?xxx????xxx?xxxx?xxxx?x????xxx????xxxx?xxxx", 118));
    if (reinterpret_cast<void*>(menu.sge_addy) == nullptr)
    {
        char failed_to_find_sge[] = "Failed to find SGE";
        HiddenText failed_to_find_sge_enc = failed_to_find_sge;
        failed_to_find_sge_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_sge_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.sge_addy));

    // Scan for network event handler
    char scanning_for_neh[] = "Scanning for NEH";
    HiddenText scanning_for_neh_enc = scanning_for_neh;
    scanning_for_neh_enc.DecryptString();
    fmt::print("{}\n", scanning_for_neh_enc.GetDecryptedString());

    char neh_sig[] = "\x66\x41\x83\xf9\xcc\x0f\x83";
    HiddenText neh_sig_enc = neh_sig;
    neh_sig_enc.DecryptString();
    menu.neh_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            neh_sig_enc.GetDecryptedString(), "xxxx?xx", 7));
    if (reinterpret_cast<void*>(menu.neh_addy) == nullptr)
    {
        char failed_to_find_neh[] = "Failed to find NEH";
        HiddenText failed_to_find_neh_enc = failed_to_find_neh;
        failed_to_find_neh_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_neh_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.neh_addy));

    // Scan for net event ack function
    char scanning_for_nea[] = "Scanning for NEA";
    HiddenText scanning_for_nea_enc = scanning_for_nea;
    scanning_for_nea_enc.DecryptString();
    fmt::print("{}\n", scanning_for_nea_enc.GetDecryptedString());

    char nea_sig[] = "\x48\x89\x5c\x24\xcc\x48\x89\x6c\x24\xcc\x48\x89\x74\x24\xcc\x57\x48\x83\xec\xcc\x80\x7a\xcc\xcc\x41\x8b\xf1";
    HiddenText nea_sig_enc = nea_sig;
    nea_sig_enc.DecryptString();
    menu.nea_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            nea_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxx?xx??xxx", 27));
    if (reinterpret_cast<void*>(menu.nea_addy) == nullptr)
    {
        char failed_to_find_nea[] = "Failed to find NEA";
        HiddenText failed_to_find_nea_enc = failed_to_find_nea;
        failed_to_find_nea_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_nea_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.nea_addy));

    // Scan for RID Writer for RID 0 protection
    char scanning_for_rid[] = "Scanning for RID";
    HiddenText scanning_for_rid_enc = scanning_for_rid;
    scanning_for_rid_enc.DecryptString();
    fmt::print("{}\n", scanning_for_rid_enc.GetDecryptedString());

    char rid_sig[] = "\x48\x89\x5C\x24\xCC\x57\x48\x83\xEC\x20\x48\x8B\xFA\x48\x8B\xD9\xE8\xCC\xCC\xCC\xCC\x48\x8B\x47\x40\x4C\x8B\xC7\x48\x8D\x4B\x64";
    //char rid_sig[] = "\xE8\x00\x00\x00\x00\x44\x89\xAB\x00\x00\x00\x00\x49\x83\xCF\xFF";
    HiddenText rid_sig_enc = rid_sig;
    rid_sig_enc.DecryptString();
    menu.rid_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            rid_sig_enc.GetDecryptedString(), "xxxx?xxxxxxxxxxxx????xxxxxxxxxxx", 32));
            //rid_sig_enc.GetDecryptedString(), "x????xxx????xxxx", 16));
    if (reinterpret_cast<void*>(menu.rid_addy) == nullptr)
    {
        char failed_to_find_rid[] = "Failed to find RID";
        HiddenText failed_to_find_rid_enc = failed_to_find_rid;
        failed_to_find_rid_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_rid_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.rid_addy));
    
    // Scan for create space for global event, call before global event creator
    char scanning_for_cev[] = "Scanning for CEV";
    HiddenText scanning_for_cev_enc = scanning_for_cev;
    scanning_for_cev_enc.DecryptString();
    fmt::print("{}\n", scanning_for_cev_enc.GetDecryptedString());

    char cev_sig[] = "\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x68\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x41\x56\x48\x83\xec\xcc\x48\x8b\x05\xcc\xcc\xcc\xcc\x48\x8b\xe9\x44\x8b\x40";
    HiddenText cev_sig_enc = cev_sig;
    cev_sig_enc.DecryptString();
    menu.cev_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            cev_sig_enc.GetDecryptedString(), "xxxxxx?xxx?xxx?xxx?xxxxx?xxx????xxxxxx", 38));
    if (reinterpret_cast<void*>(menu.cev_addy) == nullptr)
    {
        char failed_to_find_cev[] = "Failed to find CEV";
        HiddenText failed_to_find_cev_enc = failed_to_find_cev;
        failed_to_find_cev_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_cev_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.cev_addy));

    // Scan for global event creator, call before net event creator
    char scanning_for_gev[] = "Scanning for GEV";
    HiddenText scanning_for_gev_enc = scanning_for_gev;
    scanning_for_gev_enc.DecryptString();
    fmt::print("{}\n", scanning_for_gev_enc.GetDecryptedString());

    char gev_sig[] = "\x4c\x8b\xd1\x48\x63\x49\xcc\x83\xf9";
    HiddenText gev_sig_enc = gev_sig;
    gev_sig_enc.DecryptString();
    menu.gev_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            gev_sig_enc.GetDecryptedString(), "xxxxxx?xx", 9));
    if (reinterpret_cast<void*>(menu.gev_addy) == nullptr)
    {
        char failed_to_find_gev[] = "Failed to find GEV";
        HiddenText failed_to_find_gev_enc = failed_to_find_gev;
        failed_to_find_gev_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_gev_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.gev_addy));

    // Scan for add event to queue, call after creating event
    char scanning_for_aeq[] = "Scanning for AEQ";
    HiddenText scanning_for_aeq_enc = scanning_for_aeq;
    scanning_for_aeq_enc.DecryptString();
    fmt::print("{}\n", scanning_for_aeq_enc.GetDecryptedString());

    char aeq_sig[] = "\x48\x85\xd2\x0f\x84\xcc\xcc\xcc\xcc\x48\x89\x5c\x24\xcc\x48\x89\x6c\x24\xcc\x48\x89\x74\x24\xcc\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xec\xcc\x48\x8d\x71";
    HiddenText aeq_sig_enc = aeq_sig;
    aeq_sig_enc.DecryptString();
    menu.aeq_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            aeq_sig_enc.GetDecryptedString(), "xxxxx????xxxx?xxxx?xxxx?xxxxxxxxxxxx?xxx", 40));
    if (reinterpret_cast<void*>(menu.aeq_addy) == nullptr)
    {
        char failed_to_find_aeq[] = "Failed to find AEQ";
        HiddenText failed_to_find_aeq_enc = failed_to_find_aeq;
        failed_to_find_aeq_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_aeq_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.aeq_addy));

    // Scan for request door event handler
    char scanning_for_rde[] = "Scanning for RDE";
    HiddenText scanning_for_rde_enc = scanning_for_rde;
    scanning_for_rde_enc.DecryptString();
    fmt::print("{}\n", scanning_for_rde_enc.GetDecryptedString());

    char rde_sig[] = "\x48\x89\x5c\x24\xcc\x57\x48\x83\xec\xcc\x48\x8b\xc2\x48\x8d\x51";
    HiddenText rde_sig_enc = rde_sig;
    rde_sig_enc.DecryptString();
    menu.rde_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            rde_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxxxx", 16));
    if (reinterpret_cast<void*>(menu.rde_addy) == nullptr)
    {
        char failed_to_find_rde[] = "Failed to find RDE";
        HiddenText failed_to_find_rde_enc = failed_to_find_rde;
        failed_to_find_rde_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_rde_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.rde_addy));

    // Scan for request door event creator, call after creating global event, before adding event to queue
    char scanning_for_rdee[] = "Scanning for RDEE";
    HiddenText scanning_for_rdee_enc = scanning_for_rdee;
    scanning_for_rdee_enc.DecryptString();
    fmt::print("{}\n", scanning_for_rdee_enc.GetDecryptedString());

    char rdee_sig[] = "\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x41\x56\x48\x83\xec\xcc\x48\x83\x60\xcc\xcc\x49\x8b\xf1";
    HiddenText rdee_sig_enc = rdee_sig;
    rdee_sig_enc.DecryptString();
    menu.rdee_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            rdee_sig_enc.GetDecryptedString(), "xxxxxx?xxx?xxx?xxxxx?xxx??xxx", 29));
    if (reinterpret_cast<void*>(menu.rdee_addy) == nullptr)
    {
        char failed_to_find_rdee[] = "Failed to find RDEE";
        HiddenText failed_to_find_rdee_enc = failed_to_find_rdee;
        failed_to_find_rdee_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_rdee_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.rdee_addy));

    // Scan for crc hash check event creator
    char scanning_for_crce[] = "Scanning for CRCE";
    HiddenText scanning_for_crce_enc = scanning_for_crce;
    scanning_for_crce_enc.DecryptString();
    fmt::print("{}\n", scanning_for_crce_enc.GetDecryptedString());

    //char crce_sig[] = "\x48\x8b\xc4\x48\x89\x58\xcc\x48\x89\x70\xcc\x48\x89\x78\xcc\x41\x56\x48\x83\xec\xcc\x48\x83\x60\xcc\xcc\x41\x8a\xd9";
    char crce_sig[] = "\x48\x89\x5c\x24\xcc\x57\x48\x83\xec\xcc\x48\x83\x64\x24\xcc\xcc\x0f\xb7\xda\x45\x33\xc9\xba\xcc\xcc\xcc\xcc\x45\x33\xc0\x48\x8b\xf9\xe8\xcc\xcc\xcc\xcc\x48\x8d\x05\xcc\xcc\xcc\xcc\x66\x89\x5f\xcc\x48\x8b\x5c\x24\xcc\x48\x89\x07\x48\x8b\xc7";
    HiddenText crce_sig_enc = crce_sig;
    crce_sig_enc.DecryptString();
    menu.crce_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            crce_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx??xxxxxxx????xxxxxxx????xxx????xxx?xxxx?xxxxxx", 60));
    if (reinterpret_cast<void*>(menu.crce_addy) == nullptr)
    {
        char failed_to_find_crce[] = "Failed to find CRCE";
        HiddenText failed_to_find_crce_enc = failed_to_find_crce;
        failed_to_find_crce_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_crce_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.crce_addy));

    // Scan for destroy all events of type
    char scanning_for_sadv[] = "Scanning for SADV";
    HiddenText scanning_for_sadv_enc = scanning_for_sadv;
    scanning_for_sadv_enc.DecryptString();
    fmt::print("{}\n", scanning_for_sadv_enc.GetDecryptedString());

    char sadv_sig[] = "\x48\x89\x5c\x24\xcc\x48\x89\x74\x24\xcc\x48\x89\x7c\x24\xcc\x41\x56\x48\x83\xec\xcc\x48\x8b\xf2\x33\xd2";
    HiddenText sadv_sig_enc = sadv_sig;
    sadv_sig_enc.DecryptString();
    menu.sadv_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            sadv_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxxx?xxxxx", 26));
    if (reinterpret_cast<void*>(menu.sadv_addy) == nullptr)
    {
        char failed_to_find_sadv[] = "Failed to find SADV";
        HiddenText failed_to_find_sadv_enc = failed_to_find_sadv;
        failed_to_find_sadv_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_sadv_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.sadv_addy));

    // Scan for destroy all events of type
    char scanning_for_dae[] = "Scanning for DAE";
    HiddenText scanning_for_dae_enc = scanning_for_dae;
    scanning_for_dae_enc.DecryptString();
    fmt::print("{}\n", scanning_for_dae_enc.GetDecryptedString());

    char dae_sig[] = "\x48\x89\x5c\x24\xcc\x48\x89\x6c\x24\xcc\x48\x89\x74\x24\xcc\x57\x48\x83\xec\xcc\x48\x8b\x41\xcc\x0f\xb7\xea";
    HiddenText dae_sig_enc = dae_sig;
    dae_sig_enc.DecryptString();
    menu.dae_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            dae_sig_enc.GetDecryptedString(), "xxxx?xxxx?xxxx?xxxx?xxx?xxx", 27));
    if (reinterpret_cast<void*>(menu.dae_addy) == nullptr)
    {
        char failed_to_find_dae[] = "Failed to find DAE";
        HiddenText failed_to_find_dae_enc = failed_to_find_dae;
        failed_to_find_dae_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_dae_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.dae_addy));

    // Scan for get_net_player function
    char scanning_for_gnp[] = "Scanning for GNP";
    HiddenText scanning_for_gnp_enc = scanning_for_gnp;
    scanning_for_gnp_enc.DecryptString();
    fmt::print("{}\n", scanning_for_gnp_enc.GetDecryptedString());

    //char gnp_sig[] = "\x80\xF9\x20\x0F\x83\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x90"; // 1.52
   char gnp_sig[] = "\x80\xf9\xcc\x0f\x83\xcc\xcc\xcc\xcc\xe9\xcc\xcc\xcc\xcc\xa2"; // 1.53
    HiddenText gnp_sig_enc = gnp_sig;
    gnp_sig_enc.DecryptString();
    menu.gnp_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            gnp_sig_enc.GetDecryptedString(), "xx?xx????x????x", 15));
    if (reinterpret_cast<void*>(menu.gnp_addy) == nullptr)
    {
        char failed_to_find_gnp[] = "Failed to find GNP";
        HiddenText failed_to_find_gnp_enc = failed_to_find_gnp;
        failed_to_find_gnp_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_gnp_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.gnp_addy));

    // Scan for is_dlc_present
    char scanning_for_dlc[] = "Scanning for DLC";
    HiddenText scanning_for_dlc_enc = scanning_for_dlc;
    scanning_for_dlc_enc.DecryptString();
    fmt::print("{}\n", scanning_for_dlc_enc.GetDecryptedString());

    //char dlc_sig[] = "\xE8\xCC\xCC\xCC\xCC\x90\xE9\xCC\xCC\xCC\xCC\x0F\x57\xF2"; // 1.52
    char dlc_sig[] = "\xe8\xcc\xcc\xcc\xcc\x90\xe9\xcc\xcc\xcc\xcc\x48\x89\xe1"; // 1.53
    HiddenText dlc_sig_enc = dlc_sig;
    dlc_sig_enc.DecryptString();
    menu.dlc_addy = reinterpret_cast<uintptr_t>(
        PatternScan(reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
            dlc_sig_enc.GetDecryptedString(), "x????xx????xxx", 14));
    if (reinterpret_cast<void*>(menu.dlc_addy) == nullptr)
    {
        char failed_to_find_dlc[] = "Failed to find DLC";
        HiddenText failed_to_find_dlc_enc = failed_to_find_dlc;
        failed_to_find_dlc_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_dlc_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.dlc_addy));
}

bool test_stack_spoof(int a, int b, int c, int d, int e, int f)
{
    const auto ret = _ReturnAddress();
    if (static_cast<void*>(ret) < menu.gtaMod.modBaseAddr || static_cast<void*>(ret) > (menu.gtaMod.modBaseAddr + menu.gtaMod.modBaseSize))
        return false;

    fmt::print(
        "Stack spoof test: args {} + {} + {} / {} x {} - {} = {} - Return address: {}\n",
        a, b, c, d, e, f, (a+b+c/d*e-f), static_cast<void*>(ret));
    return true;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    const auto ret = spoof_call(reinterpret_cast<void*>(menu.jmp_addy), &test_stack_spoof, 1, 1, 5, 2, 498, 1923);
    if (!ret)
        return false;
    get_window_name();
    /*HWND popstar = FindWindow(0, "PopstarV");
    if (popstar != NULL)
    {
        AppWindowTitle = "PopstarV";
        ImplHookDX11_Init(hModule, popstar);
        return S_OK;
    }*/
	
	//ImplHookDX11_Init(hModule, FindWindow(0, AppWindowTitle));
    ImplHookDX11_Init(hModule, hWindow);
	return S_OK;
}

void setup()
{
    FreeConsole();
    if (AllocConsole())
    {
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        SetConsoleTitleW(L"改善");
        //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    }

    char kaizenn[] = "Kaizenn";
    HiddenText kaizennEnc = kaizenn;
    kaizennEnc.DecryptString();
    SetConsoleTitleW(L"改善");
    char allocated[] = "Allocated console";
    HiddenText allocated_enc = allocated;
    allocated_enc.DecryptString();
    fmt::print("{}\n", allocated_enc.GetDecryptedString());
	
    const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    char path[MAX_PATH + 1];
    if (!SHGetSpecialFolderPath(NULL, path, CSIDL_PROFILE, FALSE))
    {
        const HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        fmt::print("Failed to set path");
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return;
    }

    if (!log_file.is_open())
        log_file.open(fmt::format("{}\\Downloads\\Kaizenn.txt", path).c_str());
    // Grab the game module
    char gta[] = "GTA5.exe";
    HiddenText gta_enc = gta;
    gta_enc.DecryptString();
    menu.gtaMod = GetRemoteModuleInfo(0, gta_enc.GetDecryptedString());
    if (menu.gtaMod.modBaseAddr == nullptr)
    {
        char failed_to_find_gta[] = "Failed to find GTA module";
        HiddenText failed_to_find_gta_enc = failed_to_find_gta;
        failed_to_find_gta_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_gta_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    char scanning[] = "Scanning for patterns";
    HiddenText scanning_enc = scanning;
    scanning_enc.DecryptString();
    fmt::print("{}\n", scanning_enc.GetDecryptedString());

    //Scan for jmp [rbx] ROP
    char scanning_for_jmp[] = "Scanning for JMP";
    HiddenText scanning_for_jmp_enc = scanning_for_jmp;
    scanning_for_jmp_enc.DecryptString();
    fmt::print("{}\n", scanning_for_jmp_enc.GetDecryptedString());

    char jmp_sig[] = "\xFF\x23\xB1\xFC\x5F";
    HiddenText jmp_sig_enc = jmp_sig;
    jmp_sig_enc.DecryptString();
    menu.jmp_addy = reinterpret_cast<uintptr_t>(PatternScan(
        reinterpret_cast<char*>(menu.gtaMod.modBaseAddr), static_cast<uintptr_t>(menu.gtaMod.modBaseSize),
        jmp_sig_enc.GetDecryptedString(), "xxxxx", 5));
    if (reinterpret_cast<void*>(menu.jmp_addy) == nullptr)
    {
        char failed_to_find_jmp[] = "Failed to find JMP";
        HiddenText failed_to_find_jmp_enc = failed_to_find_jmp;
        failed_to_find_jmp_enc.DecryptString();
        SetConsoleTextAttribute(h_console, FOREGROUND_RED);
        cout << failed_to_find_jmp_enc.GetDecryptedString() << endl;
        SetConsoleTextAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        char dummy[1];
        cin.getline(dummy, 1);
        return;
    }

    //fmt::print("{}\n", reinterpret_cast<void*>(menu.jmp_addy));

    //HANDLE scans = spoof_call<HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD>
        (reinterpret_cast<void*>(menu.jmp_addy), &CreateThread, NULL, NULL, (LPTHREAD_START_ROUTINE)scan_for_patterns, 0, NULL, NULL);
    //WaitForSingleObject(scans, INFINITE);
    thread scans = thread(scan_for_patterns);
    scans.join();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        setup();
		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
        menu.menu_thread = spoof_call<HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD>
    	(reinterpret_cast<void*>(menu.jmp_addy), &CreateThread, NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}