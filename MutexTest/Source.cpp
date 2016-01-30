#include <windows.h>

#define MUTEX TEXT("KITTU_MUTEX")

DWORD WINAPI ThreadFunc(HWND hWnd) {
	HDC hdc;
	TCHAR str[32];
	int iCount;
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEX);

	SetWindowText(hWnd, TEXT("ミューテックスの解放を待っています"));
	WaitForSingleObject(hMutex, INFINITE);
	SetWindowText(hWnd, TEXT("処理中です..."));

	for (iCount = 0; iCount < 1000; iCount++) {
		hdc = GetDC(hWnd);
		wsprintf(str, TEXT("Count = %d"), iCount);
		TextOut(hdc, 0, 0, str, lstrlen(str));
		Sleep(10);
		ReleaseDC(hWnd, hdc);
	}
	SetWindowText(hWnd, TEXT("ミューテックスを解放しました"));

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	ExitThread(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	DWORD dwParam;
	static HANDLE hMutex;

	switch (msg) {
	case WM_DESTROY:
		if (hMutex) CloseHandle(hMutex);
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		hMutex = CreateMutex(NULL, FALSE, MUTEX);
		CreateThread(
			NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, hWnd, 0, &dwParam);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASS winc;

	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra = winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("KITTY");

	if (!RegisterClass(&winc)) return 1;

	hWnd = CreateWindow(
		TEXT("KITTY"), TEXT("Kitty on your lap"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		200, 100, NULL, NULL, hInstance, NULL
		);

	if (hWnd == NULL) return 1;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}