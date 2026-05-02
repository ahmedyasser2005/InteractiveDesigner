#include "Window.h"
#include <iostream>
#include <imgui.h>



Window::Window( std::string_view title, uint32_t width, uint32_t height )
{
	WNDCLASSEX wc{
		.cbSize = sizeof( WNDCLASSEX ),
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = HandleMsgSetup,
		.hInstance = GetModuleHandle( nullptr ),
		.hCursor = LoadCursor( nullptr, IDC_ARROW ),
		.hbrBackground = nullptr,
		.lpszClassName = className,
	};
	RegisterClassEx( &wc );

	RECT wr = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );

	std::wstring wTitle( title.begin(), title.end() );

	m_hWnd = CreateWindowEx(
		0,
		className,
		wTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		GetModuleHandle( nullptr ),
		this // Passing 'this' pointer to WM_NCCREATE
	);

	if( !m_hWnd ) return; // TODO: Handle error

	m_gfx = std::make_unique<Graphics>( m_hWnd, width, height );

	ShowWindow( m_hWnd, SW_SHOW );
}

Window::~Window() noexcept
{
	if( m_hWnd )
	{
		DestroyWindow( m_hWnd );
	}
	UnregisterClass( className, GetModuleHandle( nullptr ) );
}



// Static setup function: Grabs the 'this' pointer passed in CreateWindowEx
LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	if( msg == WM_NCCREATE )
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgThunk) );
		return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

// Thunk function: Redirects all future messages to the correct C++ instance
LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd, GWLP_USERDATA ));
	return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}



// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT Window::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return 0;

	switch( msg )
	{
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}



		// Keyboard Messages
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			input.keys[wParam] = true;
			return 0;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			input.keys[wParam] = false;
			return 0;
		}
		// End of Keyboard Messages



		// Mouse Messages
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			input.x = static_cast<int>(pt.x);
			input.y = static_cast<int>(pt.y);
			return 0;
		}
		case WM_LBUTTONDOWN: { input.leftDown = true;	 return 0; }
		case WM_LBUTTONUP: { input.leftDown = false; return 0; }

		case WM_RBUTTONDOWN: { input.rightDown = true;	 return 0; }
		case WM_RBUTTONUP: { input.rightDown = false; return 0; }

		case WM_MBUTTONDOWN: { input.middleDown = true;	 return 0; }
		case WM_MBUTTONUP: { input.middleDown = false; return 0; }
		case WM_MOUSEWHEEL:
		{
			input.wheelDelta += GET_WHEEL_DELTA_WPARAM( wParam );
			return 0;
		}
		// End of Mouse Messages
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_QUIT )
		{
			return (int)msg.wParam;
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return {}; // Continue running
}