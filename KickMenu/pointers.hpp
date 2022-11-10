#pragma once
#include <network/Network.hpp>
#include <network/CNetGamePlayer.hpp>
#include "network/CNetworkPlayerMgr.hpp"
#include "common.hpp"
#include "function_types.hpp"
#include "socialclub/FriendRegistry.hpp"
#include "natives.hpp"

namespace big
{
	class pointers
	{
	public:
		explicit pointers();
		~pointers();
	public:
		HWND m_hwnd{};

		Network** m_network;
		CNetworkPlayerMgr** m_network_player_mgr{};
		FriendRegistry* m_friend_registry{};

		IDXGISwapChain** m_swapchain{};
		
		functions::get_native_handler m_get_native_handler{};
		rage::scrNativeRegistrationTable* m_native_registration_table{};
		functions::fix_vectors m_fix_vectors{};

		PVOID m_native_return;

		functions::get_connection_peer m_get_connection_peer;
		functions::send_remove_gamer_cmd m_send_remove_gamer_cmd;
		functions::handle_remove_gamer_cmd m_handle_remove_gamer_cmd;
	};

	inline pointers* g_pointers{};
}