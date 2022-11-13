#pragma once
#include <network/Network.hpp>
#include <network/CNetGamePlayer.hpp>
#include "network/CNetworkPlayerMgr.hpp"
#include "common.hpp"
#include "function_types.hpp"
#include "socialclub/FriendRegistry.hpp"

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

		//you can get this pointer to use D3D11CreateDeviceAndSwapChain,but Yim is use feature Code,I don't know why him do that
		IDXGISwapChain** m_swapchain{};

		functions::get_connection_peer m_get_connection_peer;
		functions::send_remove_gamer_cmd m_send_remove_gamer_cmd;
		functions::handle_remove_gamer_cmd m_handle_remove_gamer_cmd;
	};

	inline pointers* g_pointers{};
}