#pragma once

#include "Application/Application.h"

#ifdef __cplusplus
extern "C" {
#endif

	APP_API PAppProcTable* GetAppProcTable();

	APP_API PAppWindowId APP_Create_Window_Win32(const char* title_name, int x, int y, uint32_t width, uint32_t height, uint32_t flags);
	
	APP_API bool APP_Poll_Event(PAppEvent* event);

	typedef struct PAppWindow_Win32
	{
		PAppWindow super;
		HWND hwnd;
		HINSTANCE hinst;
	}PAppWindow_Win32;

	typedef struct PAppEvent_Win32
	{
		PAppEvent super;
		MSG msg;
	}PAppEvent_Win32;

#ifdef __cplusplus
}
#endif