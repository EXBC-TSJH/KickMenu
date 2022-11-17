#include "common.hpp"
#include "pointers.hpp"
#include "memory/all.hpp"

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

		// Swapchain
		main_batch.add("S", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", [this](memory::handle ptr)
		{
			m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
		});

		// Native Handlers
		main_batch.add("NH", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [this](memory::handle ptr)
		{
			m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
			m_get_native_handler = ptr.add(12).rip().as<functions::get_native_handler>();
		});

		// Fix Vectors
		main_batch.add("FV", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [this](memory::handle ptr)
		{
			m_fix_vectors = ptr.as<functions::fix_vectors>();
		});

		// Native Return Spoofer
		main_batch.add("NRF", "FF E3", [this](memory::handle ptr)
		{
			m_native_return = ptr.add(0).as<PVOID>();
		});
//-------------------------------------------------------------------------------------------------------

		auto mem_region = memory::module(nullptr);
		main_batch.run(mem_region);
//-------------------------------------------------------------------------------------------------------
		/**
		* Freemode thread restorer through VM patch
		*/
		
		if (auto pat1 = mem_region.scan("3b 0a 0f 83 ? ? ? ? 48 ff c7"))
		{
			memory::byte_patch::make(pat1.add(2).as<uint32_t*>(), 0xc9310272)->apply();
			memory::byte_patch::make(pat1.add(6).as<uint16_t*>(), 0x9090)->apply();
		}

		if (auto pat2 = mem_region.scan("3b 0a 0f 83 ? ? ? ? 49 03 fa"))
		{
			memory::byte_patch::make(pat2.add(2).as<uint32_t*>(), 0xc9310272)->apply();
			memory::byte_patch::make(pat2.add(6).as<uint16_t*>(), 0x9090)->apply();
		}

		auto pat3 = mem_region.scan_all("3b 11 0f 83 ? ? ? ? 48 ff c7");
		for (auto& handle : pat3)
		{
			memory::byte_patch::make(handle.add(2).as<uint32_t*>(), 0xd2310272)->apply();
			memory::byte_patch::make(handle.add(6).as<uint16_t*>(), 0x9090)->apply();
		}

		auto pat4 = mem_region.scan_all("3b 11 0f 83 ? ? ? ? 49 03 fa");
		for (auto& handle : pat4)
		{
			memory::byte_patch::make(handle.add(2).as<uint32_t*>(), 0xd2310272)->apply();
			memory::byte_patch::make(handle.add(6).as<uint16_t*>(), 0x9090)->apply();
		}
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
