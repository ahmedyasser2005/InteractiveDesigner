#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>

class Window {
public:
	Window( int width, int height, const std::wstring& title );
	~Window();

	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;

	bool ProcessMessages();
	HWND GetHandle() const { return m_hwnd; }

private:
	static LRESULT CALLBACK HandleMsgSetup( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK HandleMsgThunk( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT HandleMsg( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	HINSTANCE m_hInst;
	HWND m_hwnd;
	const wchar_t* className = L"InteractiveDesignerWindowClass";
	int m_width;
	int m_height;
};