#pragma once
#include <Windows.h>
#include <dxgi.h>
#include "common.hpp"
#include "natives.hpp"

#define MENU_TOGGLE VK_INSERT
#define UNLOAD_TOGGLE VK_END

typedef HRESULT(_cdecl *old_present)(IDXGISwapChain*, UINT, UINT);

BOOL Init_UI();
void UnLoadUI();
LRESULT WINAPI wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags);
void present();
void DrawUi();

void BlockMouseMessageToGTA5();