#include "Application/Windows/Application_Win32.h"

const PAppProcTable g_win32_ProcTable =
{
	.pfn_AppCreateWindow = APP_Create_Window_Win32,
	.pfn_AppPollEvent = APP_Poll_Event
};

PAppProcTable* GetAppProcTable()
{
	return &g_win32_ProcTable;
}

