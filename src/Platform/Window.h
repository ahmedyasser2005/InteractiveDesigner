#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

class Window {
public:
	Window( int width, int height, const std::wstring& title );
	~Window();

	// Prevent copying to avoid multiple windows managing the same HWND
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
	int m_width;
	int m_height;
};