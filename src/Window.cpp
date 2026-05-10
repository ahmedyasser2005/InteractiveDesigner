#include "Window.h"
#include "GeoUtils.h"
#include <imgui.h>

Window::Window( std::string_view title, uint32_t width, uint32_t height, Input* input )
	: m_input( input )
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

	RECT wr = { 0L, 0L, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );

	std::wstring wTitle( title.begin(), title.end() );

	m_hWnd = CreateWindowEx(
		0UL,
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
		return 0LL;

	switch( msg )
	{
		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0LL;
		}



		// Keyboard Messages
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			m_input->keys[wParam] = true;
			return 0LL;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			m_input->keys[wParam] = false;
			return 0LL;
		}
		// End of Keyboard Messages



		// Mouse Messages
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS( lParam );
			m_input->pos = { static_cast<float>(pt.x), static_cast<float>(pt.y) };
			return 0LL;
		}
		case WM_LBUTTONDOWN: { m_input->leftDown = true;	 return 0LL; }
		case WM_LBUTTONUP: { m_input->leftDown = false; return 0LL; }

		case WM_RBUTTONDOWN: { m_input->rightDown = true;	 return 0LL; }
		case WM_RBUTTONUP: { m_input->rightDown = false; return 0LL; }

		case WM_MBUTTONDOWN: { m_input->middleDown = true;	 return 0LL; }
		case WM_MBUTTONUP: { m_input->middleDown = false; return 0LL; }
		case WM_MOUSEWHEEL:
		{
			m_input->wheelDelta += GET_WHEEL_DELTA_WPARAM( wParam );
			return 0LL;
		}
		// End of Mouse Messages
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	while( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
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
