#include "Window.h"

Window::Window( int width, int height, const std::wstring& title )
	: m_hInst( GetModuleHandle( nullptr ) ), m_width( width ), m_height( height )
{
	SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );

	const wchar_t* className = L"InteractiveDesignerWindowClass";
	WNDCLASSW wc = {
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = HandleMsgSetup, // First call goes here
		.hInstance = m_hInst,
		.hCursor = LoadCursor( nullptr, IDC_ARROW ),
		.lpszClassName = className
	};
	RegisterClassW( &wc );

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE );

	m_hwnd = CreateWindowExW(
		0, className, title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, m_hInst, this // Pass 'this' to the creation params
	);

	ShowWindow( m_hwnd, SW_SHOW );
}

Window::~Window()
{
	UnregisterClassW( L"InteractiveDesignerWindowClass", m_hInst );
	DestroyWindow( m_hwnd );
}

LRESULT CALLBACK Window::HandleMsgSetup( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_NCCREATE )
	{
		// Extract our 'this' pointer from the creation params
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// Store it in the Win32 user data for future retrieval
		SetWindowLongPtrW( hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
		// Switch the procedure to the "Thunk" version
		SetWindowLongPtrW( hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
		return pWnd->HandleMsg( hwnd, uMsg, wParam, lParam );
	}
	return DefWindowProcW( hwnd, uMsg, wParam, lParam );
}

LRESULT CALLBACK Window::HandleMsgThunk( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Retrieve our 'this' pointer and call the member function
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtrW( hwnd, GWLP_USERDATA ));
	return pWnd->HandleMsg( hwnd, uMsg, wParam, lParam );
}

LRESULT Window::HandleMsg( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProcW( hwnd, uMsg, wParam, lParam );
}

bool Window::ProcessMessages()
{
	MSG msg;
	while( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
	{
		if( msg.message == WM_QUIT ) return false;
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}
	return true;
}