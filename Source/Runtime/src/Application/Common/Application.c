#include "Application/Application.h"

#ifdef _WIN32 || _WIN64
	#include "Application/Windows/Application_Win32.h"
#endif

const PAppProcTable* gs_proc_table = APP_NULL;

bool AppInitialize()
{
	if (!gs_proc_table)
	{
#ifdef _WIN32 || _WIN64
		{
			gs_proc_table = GetAppProcTable();
		}
#endif
#ifdef __linux__
		{

		}
#endif
#ifdef __APPLE__
		{

		}
#endif
	}

	if (!gs_proc_table)
		return 0;
	return 1;
}

bool AppTerminal()
{
	gs_proc_table = APP_NULL;
	return 1;
}

PAppWindowId APPCreateWindow(const char* title_name, int x, int y, uint32_t width, uint32_t height, uint32_t flags)
{
	PAppWindowId app = gs_proc_table->pfn_AppCreateWindow(title_name,x,y,width,height,flags);
	return app;
}

bool APPPollEvent()
{
	return 1;
}
