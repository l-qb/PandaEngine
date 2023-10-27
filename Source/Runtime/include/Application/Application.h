#pragma once

#include "Application_config.h"
#include "Application_Flags.h"


#define DECLARE_APP_OBJECT(classname) struct classname##descriptor; typedef const struct classname* classname##Id;

DECLARE_APP_OBJECT(PAppWindow)

#ifdef __cplusplus
extern "C" {
#endif

	/*virtual function list*/
	APP_API bool AppInitialize();
	APP_API bool AppTerminal();

	APP_API PAppWindowId APPCreateWindow(const char* title_name, int x, int y, uint32_t width, uint32_t height, uint32_t flags);
	typedef PAppWindowId(*PFN_APPCreateWindow)(const char* title_name, int x, int y, uint32_t width, uint32_t height, uint32_t flags);

	APP_API bool APPPollEvent(struct PAppEvent* event);
	typedef bool(*PFN_APPPollEvent)(struct PAppEvent* event);

	typedef struct PAppProcTable
	{
		const PFN_APPCreateWindow pfn_AppCreateWindow;
		const PFN_APPPollEvent pfn_AppPollEvent;

	}PAppProcTable;
	static const PAppProcTable* gs_proc_table;

	/* app object */
	typedef struct PAppWindow
	{
		uint32_t width;
		uint32_t height;
	}PAppWindow;

	typedef struct PAppEvent
	{
		DWORD type;
	}PAppEvent;

#ifdef __cplusplus
}
#endif