#include "UI.hpp"
#include "MyUtils.hpp"

#include "common.hpp"

#include "Kick.h"
#include "player_service.hpp"
#include "player.hpp"
#include "pointers.hpp"
#include "vmt_hook.hpp"
#include "natives.hpp"
#include "invoker.hpp"
#include "gta_util.hpp"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include <d3d12.h>
#include <dxgi1_4.h>

#include <Windows.h>
#include <map>
#include <iostream>
#include <string>
//-------------------------------------------------------------
WNDPROC OriginalWndProc;
old_present Original_swapchain_present;

inline std::atomic_bool Menu_opened{ false };
bool for_close_button = false;

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DISABLE_CONTROL_ACTION(int padIndex, int control, BOOL disable) { invoke<void>(0xFE99B66D079CF6BC, padIndex, control, disable); } // 0xFE99B66D079CF6BC 0x3800C0DC b323

big::comptr<IDXGISwapChain> dxgi_swapchain;
big::comptr<ID3D11Device> d3d_device;
big::comptr<ID3D11DeviceContext> d3d_device_context;

big::vmt_hook *p_swapchain_hook = NULL;
//-------------------------------------------------------------
BOOL Init_UI()
{	
//++++++++++++++++++++++++++++Init ImGui+++++++++++++++++++++++++++++++++
	{
		dxgi_swapchain = big::comptr<IDXGISwapChain>(*big::g_pointers->m_swapchain);

		void* tmp_d3d_device{};
		if (SUCCEEDED(dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &tmp_d3d_device)))
		{
			d3d_device.Attach(static_cast<ID3D11Device*>(tmp_d3d_device));
		}
		else
		{
			throw std::runtime_error("Failed to get D3D device.");
		}

		d3d_device->GetImmediateContext(d3d_device_context.GetAddressOf());

		ImGuiContext* ctx = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		static char str[] = "./imgui.ini";
		ctx->IO.IniFilename = str;

		//ImGui::StyleColorsDark();
		ImGui::StyleColorsLight();

		io.Fonts->AddFontDefault();
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simsun.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		IM_ASSERT(font != NULL);

		io.FontDefault = font;

		auto Init_DX11 = ImGui_ImplDX11_Init(d3d_device.Get(), d3d_device_context.Get());
		auto Init_Win32 = ImGui_ImplWin32_Init(big::g_pointers->m_hwnd);
		
		Menu_opened = true;
		for_close_button = true;
	}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	big::g_native_invoker.cache_handlers();	//script

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (big::g_pointers) 
	{
		static big::vmt_hook m_swapchain_hook(*big::g_pointers->m_swapchain, swapchain_num_funcs);
		m_swapchain_hook.hook(swapchain_present_index, &swapchain_present);
		m_swapchain_hook.enable();
		Original_swapchain_present = m_swapchain_hook.get_original<decltype(&swapchain_present)>(swapchain_present_index);

		p_swapchain_hook = &m_swapchain_hook;
	}
	else { return FALSE; }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	OriginalWndProc = WNDPROC(SetWindowLongPtrW(big::g_pointers->m_hwnd, GWLP_WNDPROC, LONG_PTR(&wndProc)));
	if (!OriginalWndProc)
		return FALSE;

	return TRUE;
}
//-------------------------------------------------------------
void UnLoadUI()
{	
	Menu_opened = false;

	p_swapchain_hook->disable();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	dxgi_swapchain.Reset();
	d3d_device.Reset();
	d3d_device_context.Reset();

	WNDPROC(SetWindowLongPtrW(big::g_pointers->m_hwnd, GWLP_WNDPROC, LONG_PTR(OriginalWndProc)));
}
//-------------------------------------------------------------
LRESULT WINAPI wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYUP && wParam == MENU_TOGGLE && big::g_running)
		Menu_opened = !Menu_opened;

	if (!for_close_button)
		big::g_running = false;

	if (Menu_opened && for_close_button)
		BlockMouseMessageToGTA5();

	if(Menu_opened && for_close_button)
		ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);

	return CallWindowProcW(OriginalWndProc,window, msg, wParam, lParam);
}
//-------------------------------------------------------------
//vmthook
HRESULT swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags)
{
	if (big::g_running)
		present();

	return Original_swapchain_present(this_, sync_interval, flags);
}
//-------------------------------------------------------------
void present()
{
	if (Menu_opened)
	{
		ImGui::GetIO().MouseDrawCursor = true;
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
	}
	else
	{
		ImGui::GetIO().MouseDrawCursor = false;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	if(Menu_opened && for_close_button)	
		DrawUi();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
//-------------------------------------------------------------
bool set_wndow_pos()
{
	ImGui::SetWindowPos({ 480.f, 184.f });
	return true;
}
void DrawUi()
{
	ImGui::SetWindowSize({ 935.f, 583.f });
	auto onec = set_wndow_pos();

	big::g_player_service->UpdataList();
	auto players = big::g_player_service->players();
	int count = 0;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	
	ImGui::Begin("KickMenu",&for_close_button ,window_flags);

	std::map<std::string, big::player_ptr>::iterator iter;
	for (iter = players.begin(); iter != players.end(); iter++)
	{
		if (ImGui::Button(iter->second.get()->get_name(), ImVec2(300.0f, 50.0f)))
			Kick::breakup_kick(iter->second);
		
		if (count < 3 - 1)
		{
			count++;
			ImGui::SameLine();
		}	
		else 
		{
			count = 0;
		}	
	}
	
	ImGui::End();
}
//-------------------------------------------------------------
void BlockMouseMessageToGTA5()
{
	for (uint8_t i = 0; i <= 6; i++)
		DISABLE_CONTROL_ACTION(2, i, true);

	DISABLE_CONTROL_ACTION(2, 106, true);
	DISABLE_CONTROL_ACTION(2, 329, true);
	DISABLE_CONTROL_ACTION(2, 330, true);

	DISABLE_CONTROL_ACTION(2, 14, true);
	DISABLE_CONTROL_ACTION(2, 15, true);
	DISABLE_CONTROL_ACTION(2, 16, true);
	DISABLE_CONTROL_ACTION(2, 17, true);
	DISABLE_CONTROL_ACTION(2, 24, true);
	DISABLE_CONTROL_ACTION(2, 69, true);
	DISABLE_CONTROL_ACTION(2, 70, true);
	DISABLE_CONTROL_ACTION(2, 84, true);			
	DISABLE_CONTROL_ACTION(2, 85, true);
	DISABLE_CONTROL_ACTION(2, 99, true);
	DISABLE_CONTROL_ACTION(2, 92, true);
	DISABLE_CONTROL_ACTION(2, 100, true);
	DISABLE_CONTROL_ACTION(2, 114, true);
	DISABLE_CONTROL_ACTION(2, 115, true);
	DISABLE_CONTROL_ACTION(2, 121, true);
	DISABLE_CONTROL_ACTION(2, 142, true);
	DISABLE_CONTROL_ACTION(2, 241, true);
	DISABLE_CONTROL_ACTION(2, 261, true);
	DISABLE_CONTROL_ACTION(2, 257, true);
	DISABLE_CONTROL_ACTION(2, 262, true);
	DISABLE_CONTROL_ACTION(2, 331, true);
}
//-------------------------------------------------------------