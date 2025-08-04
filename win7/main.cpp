#include <Windows.h>
#include <stdio.h>
#include <io.h>

#define MAX_LEN 256
#define WINDOW_CLASS L"ChatToolsNotify"

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
	if (msg == WM_DESTROY) {
		NOTIFYICONDATA delData = {};
		delData.cbSize = sizeof(NOTIFYICONDATA);
		delData.hWnd = hwnd;
		delData.uID = 0;
		Shell_NotifyIcon(NIM_DELETE, &delData);
		PostQuitMessage(0);
	}
	return DefWindowProc(hwnd, msg, param, lparam);
}

void fatal_error(const PCWSTR cat, DWORD id) {
	WCHAR buf[MAX_LEN] = {};
	swprintf_s(buf, MAX_LEN, L"Error:%s\nID:%d\nLINE:%d\n", cat, id, __LINE__);
#ifdef _DEBUG
	MessageBox(NULL, buf, L"ERR", MB_ICONERROR);
#else
	OutputDebugString(buf);
#endif
	puts("SEND_FAILED");
	exit(0);
}

HWND init_win(HINSTANCE inst) {
	WNDCLASS wc = {};
	HWND hwnd = NULL;
	DWORD err = 0;
	wc.hInstance = inst;
	wc.lpszClassName = WINDOW_CLASS;
	wc.lpfnWndProc = wndproc;
	ATOM classAtom = RegisterClass(&wc);
	err = GetLastError();
	if (err == 0) {
		hwnd = CreateWindow(WINDOW_CLASS, WINDOW_CLASS, WS_OVERLAPPED | WS_SYSMENU, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, inst, 0);
		err = GetLastError();
		if (err != 0) fatal_error(L"WIN", err);
		if (!UpdateWindow(hwnd)) fatal_error(L"APP", 0);
	}
	else fatal_error(L"WIN", err);
	return hwnd;
}

HICON init_icon(HINSTANCE inst, const WCHAR* path) {
	HICON icon;
	WCHAR absPath[MAX_LEN] = {};
	if (_wfullpath(absPath, path ? path : L".\\icon1.ico", MAX_LEN) != NULL && _waccess_s(absPath, 0) == 0) {
		icon = (HICON)LoadImage(inst, absPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		if (!icon) {
			OutputDebugString(L"Invalid icon file");
			goto fail;
		}
		return icon;
	}
	else {
		OutputDebugString(L"Invalid icon path");
		goto fail;
	}
fail:
	icon = LoadIcon(0, IDI_APPLICATION);
	return icon;
}

void show_notify(HWND hwnd, HICON icon, PCWSTR title, PCWSTR msg, unsigned int time) {
	NOTIFYICONDATA addData = {};
	NOTIFYICONDATA showData = {};
	addData.cbSize = sizeof(NOTIFYICONDATA);
	addData.uID = 0;
	addData.hWnd = hwnd;
	addData.hIcon = icon;
	wcscpy_s(addData.szTip, 128, L"Chat Tools");
	addData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	addData.uCallbackMessage = WM_USER + 20;
	addData.uTimeout = 200;
	Shell_NotifyIcon(NIM_ADD, &addData);
	showData.cbSize = sizeof(NOTIFYICONDATA);
	showData.uID = 0;
	showData.hWnd = hwnd;
	showData.hIcon = icon;
	showData.uFlags = NIF_INFO;
	wcscpy_s(showData.szTip, 128, L"Balloon Tooltip");
	wcscpy_s(showData.szInfoTitle, 64, title);
	wcscpy_s(showData.szInfo, 256, msg);
	showData.uTimeout = 200;
	Shell_NotifyIcon(NIM_MODIFY, &showData);
	Sleep(time * 1000);
}

void release(HWND hwnd, HICON icon, HINSTANCE inst) {
	DestroyWindow(hwnd);
	DestroyIcon(icon);
	UnregisterClass(WINDOW_CLASS, inst);
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
	HINSTANCE inst = GetModuleHandle(NULL);
	LPWSTR iconpath = NULL;
	LPWSTR title = NULL;
	LPWSTR text = NULL;
	if (argc >= 3) {
		title = argv[1];
		text = argv[2];
	}
	if (argc >= 4) {
		iconpath = argv[3];
	}

	HWND hwnd = init_win(inst);
	HICON icon = init_icon(inst, iconpath);
	if (title && text) {
		show_notify(hwnd, icon, title, text, 5);
	}
	else {
		show_notify(hwnd, icon, L"Hello", L"world", 5);
	}
	release(hwnd, icon, inst);
	puts("SEND_OK");
	return 0;
}