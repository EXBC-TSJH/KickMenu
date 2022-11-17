#include <string>

#include "pointers.hpp"
#include "player.hpp"
#include "Kick.h"
#include "UI.hpp"

#include "MyUtils.hpp"
//------------------------------------------------------------------------------
DWORD WINAPI OnDllAttach(LPVOID base);

HANDLE g_main_thread = NULL;
HMODULE g_hmodule = NULL;
//------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) 
    {
        DisableThreadLibraryCalls(hModule);

        g_hmodule = hModule;
        g_main_thread = CreateThread(nullptr, 0, OnDllAttach, hModule, 0, nullptr);
    }
    
    return TRUE;
}
//------------------------------------------------------------------------------
DWORD WINAPI OnDllAttach(LPVOID base)
{
    while (!FindWindow(L"grcWindow", L"Grand Theft Auto V"))
        Sleep(1000);

    big::g_running = true;

    auto pointers_instance = std::make_unique<big::pointers>();
    auto player_service_instance = std::make_unique<big::player_service>();
    
    if (!Init_UI())
    {
        big::g_running = false;
    }

    while (big::g_running)
        Sleep(1000);

    UnLoadUI();

    player_service_instance.reset();
    pointers_instance.reset();

    CloseHandle(g_main_thread);
    FreeLibraryAndExitThread(g_hmodule, 0);
}