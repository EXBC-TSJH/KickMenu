#pragma once
#include "ped/CPedFactory.hpp"
#include "pointers.hpp"

// Spin lock flag for EnterSpinLock()/LeaveSpinLock().
extern volatile LONG g_isLocked;

namespace big::gta_util
{
    inline CNetworkPlayerMgr* get_network_player_mgr()
    {
        return *g_pointers->m_network_player_mgr;
    }

    inline Network* get_network()
    {
        return *g_pointers->m_network;
    }

    static VOID EnterSpinLock(VOID)
    {
        SIZE_T spinCount = 0;

        // Wait until the flag is FALSE.
        while (InterlockedCompareExchange(&g_isLocked, TRUE, FALSE) != FALSE)
        {
            // No need to generate a memory barrier here, since InterlockedCompareExchange()
            // generates a full memory barrier itself.

            // Prevent the loop from being too busy.
            if (spinCount < 32)
                Sleep(0);
            else
                Sleep(1);

            spinCount++;
        }
    }

    //-------------------------------------------------------------------------
    static VOID LeaveSpinLock(VOID)
    {
        // No need to generate a memory barrier here, since InterlockedExchange()
        // generates a full memory barrier itself.

        InterlockedExchange(&g_isLocked, FALSE);
    }
}
