#include "Application/Windows/Application_Win32.h"

LRESULT WINAPI Win32MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 1;
		}

		default:
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

PAppWindowId APP_Create_Window_Win32(const char* title_name, int x, int y, uint32_t width, uint32_t height, uint32_t flags)
{
	PAppWindow_Win32* app = (PAppWindow_Win32*)calloc(1, sizeof(PAppWindow_Win32));
	memset(app, 0, sizeof(PAppWindow_Win32));


	app->hinst = GetModuleHandleA(0);

	int name_len = MultiByteToWideChar(CP_UTF8, 0, title_name, -1, 0, 0);
	wchar_t* w_title_name = (wchar_t*)malloc(name_len * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, title_name, -1, w_title_name, name_len);

	WNDCLASSEX	wndclass;
	HWND		   hWnd = NULL;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc = Win32MsgProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = app->hinst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"PandaEngine";
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (RegisterClassEx(&wndclass) == 0)
		return 0;

	// create window
	if (!(hWnd = CreateWindowEx(
		NULL, 
		L"PandaEngine",
		w_title_name,
		WS_OVERLAPPEDWINDOW & (~WS_OVERLAPPED) | WS_VISIBLE,
		x, y, width, height, 
		NULL, NULL, app->hinst, NULL)))
		return 0;

	free(w_title_name);
	app->hwnd = hWnd;

	return &(app->super);
}

bool APP_Poll_Event(PAppEvent* event)
{
	return 1;
}

