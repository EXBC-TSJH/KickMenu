#include "common.hpp"
#include "pointers.hpp"
#include "memory/all.hpp"
#include "MyUtils.hpp"

namespace big
{
	pointers::pointers()
	{
		memory::batch main_batch;
//-------------------------------------------------------------------------------------------------------
		// Get Connection Peer & Send Remove Gamer Command
		main_batch.add("GCP&SRGC", "8D 42 FF 83 F8 FD 77 3D", [this](memory::handle ptr)
		{
			m_get_connection_peer = ptr.add(23).rip().as<functions::get_connection_peer>();
			m_send_remove_gamer_cmd = ptr.add(65).rip().as<functions::send_remove_gamer_cmd>();
		});

		// Handle Remove Gamer Command
		main_batch.add("HRGC", "41 FF C6 FF C7", [this](memory::handle ptr)
		{
			m_handle_remove_gamer_cmd = ptr.sub(0x6E).as<functions::handle_remove_gamer_cmd>();
		});

		// Network
		main_batch.add("N", "48 8B 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 84 C0 75 17 48 8B 0D ? ? ? ? 48 8B D7", [this](memory::handle ptr)
		{
			m_network = ptr.add(3).rip().as<Network**>();
		});

		// Network Player Manager
		main_batch.add("NPM", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [this](memory::handle ptr)
		{
			m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
		});

		// FriendRegistry
		main_batch.add("FR", "3B 0D ? ? ? ? 73 17", [this](memory::handle ptr)
		{
			m_friend_registry = ptr.add(2).rip().as<FriendRegistry*>();
		});

		// Swapchain , 一个虚函数表的指针,可以通过D3D11CreateDeviceAndSwapChain函数获取
		main_batch.add("S", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", [this](memory::handle ptr)
		{
			m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
		});

//-------------------------------------------------------------------------------------------------------
		auto mem_region = memory::module(nullptr);
		main_batch.run(mem_region);
//-------------------------------------------------------------------------------------------------------
		m_hwnd = FindWindowW(L"grcWindow", nullptr);

		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}
//-------------------------------------------------------------------------------------------------------
	pointers::~pointers()
	{
		memory::byte_patch::restore_all();

		g_pointers = nullptr;
	}
}
