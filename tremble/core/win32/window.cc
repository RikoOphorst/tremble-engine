#include "window.h"
#include "../game_manager.h"
#include "../get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Window::Window(const std::string& name, unsigned int width, unsigned int height, GameManager* game_manager) :
		width_(width),
		height_(height),
		game_manager_(game_manager),
		has_focus_(false)
	{
		if (Get::Config().fullscreen)
		{
			width_ = width = GetSystemMetrics(SM_CXSCREEN);
			height_ = height = GetSystemMetrics(SM_CYSCREEN);
		}

		WNDCLASSA wndclass;
		hinstance_ = GetModuleHandle(0);
		ZeroMemory(&wndclass, sizeof(WNDCLASSA));
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc = WindowProc;
		wndclass.hInstance = hinstance_;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wndclass.lpszClassName = name.c_str();
		wndclass.cbWndExtra = sizeof(void*);
		wndclass.cbClsExtra = sizeof(WNDCLASSA);

		ATOM res = RegisterClassA(&wndclass);

		style_ = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		if (Get::Config().fullscreen)
			style_ = 0;

		RECT client_rect;
		client_rect.left = client_rect.top = 0;
		client_rect.right = width;
		client_rect.bottom = height;

		AdjustWindowRect(&client_rect, style_, FALSE);
		int actualWidth = client_rect.right - client_rect.left;
		int actualHeight = client_rect.bottom - client_rect.top;
		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - actualWidth / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - actualHeight / 2;

		hwindow_ = CreateWindowA(wndclass.lpszClassName, name.c_str(), style_, x, y, actualWidth, actualHeight, GetDesktopWindow(), NULL, wndclass.hInstance, this);

		//bind window quit function to game manager's stop running function
		quit_function_ = std::bind(&GameManager::StopRunning, game_manager_);

		//Get rid of the bar
		if (Get::Config().fullscreen)
		{
			SetWindowLong(hwindow_, GWL_STYLE, 0);
		}

		ShowWindow(hwindow_, SW_SHOW);
	}

	//------------------------------------------------------------------------------------------------------
	Window::~Window()
	{
		DestroyWindow(hwindow_);
	}

	//------------------------------------------------------------------------------------------------------
	LRESULT CALLBACK Window::WindowProc(HWND hwindow, UINT message, WPARAM wparam, LPARAM lparam)
	{
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtrA(hwindow, GWLP_USERDATA));

		if (message == WM_CREATE)
		{
			SetWindowLongPtrA(hwindow, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lparam)->lpCreateParams));
			return DefWindowProcA(hwindow, message, wparam, lparam);
		}

		else if (message == WM_CLOSE)
		{
			window->quit_function_();
		}

		else if (message == WM_KILLFOCUS)
		{
			window->has_focus_ = false;
		}

		else if (message == WM_SETFOCUS)
		{
			window->has_focus_ = true;
		}

		else if (message == WM_ACTIVATE)
		{
			if (LOWORD(wparam) == WA_ACTIVE || LOWORD(wparam) == WA_CLICKACTIVE)
			{
				window->has_focus_ = true;
			}
			else
			{
				window->has_focus_ = false;
			}
		}

		return DefWindowProcA(hwindow, message, wparam, lparam);
	}

	//------------------------------------------------------------------------------------------------------
	void Window::GetPosition(int& x, int& y) const
	{
		POINT zero = POINT{ 0, 0 };
		ClientToScreen(hwindow_, &zero);
		//AdjustWindowRect(&window_rect, style_, FALSE);
		x = zero.x;
		y = zero.y;
	}

	//------------------------------------------------------------------------------------------------------
	void Window::ProcessMessages()
	{
		MSG msg;

		while (PeekMessageA(&msg, hwindow_, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);

			process_input_messages_(msg);
		}
	}
}