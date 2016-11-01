#pragma once
#ifdef WIN32

#include <windows.h>
#include <string>
using std::string;

class BasicMsgProc
{
public:
	static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lPrarm) {
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lPrarm);

	}
};
class WindowsPlatform {
public:
	void MakeWindow() {
		WNDCLASS wndclass = { 0 };
		DWORD    wStyle = 0;
		RECT     windowRect;
		HINSTANCE hInstance = GetModuleHandle(NULL);
		wndclass.style = CS_OWNDC;
		wndclass.lpfnWndProc = (WNDPROC)BasicMsgProc::MsgProc;
		wndclass.hInstance = hInstance;
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		static int i = 0;
		char b = 'a' + i;
		string className = "opengles3.0";
		className += b;
		wndclass.lpszClassName = className.c_str();
		i++;
		if (!RegisterClass(&wndclass)) {
			MessageBox(NULL, TEXT("create window fail"), TEXT("error"), MB_OK);
			exit(0);
		}
		wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

		// Adjust the window rectangle so that the client area has
		// the correct number of pixels
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = width;
		windowRect.bottom = height;

		AdjustWindowRect(&windowRect, wStyle, FALSE);
		hWnd = CreateWindow(
			className.c_str(),
			className.c_str(),
			wStyle,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			hInstance,
			NULL);
		//SetWindowLongPtr (  hWnd, GWL_USERDATA, (LONG) (LONG_PTR) esContext );


		if (hWnd == NULL)
		{
			MessageBox(NULL, TEXT("create window fail"), TEXT("error"), MB_OK);
			exit(0);
		}

		ShowWindow(hWnd, TRUE);
	}



	HWND hWnd;
	int width = 1280;
	int height = 720;
};

#endif // WIN32