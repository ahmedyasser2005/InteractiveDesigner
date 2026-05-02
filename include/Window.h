#pragma once
#include "CustomWin.h"
#include "Graphics.h"
#include "Input.h"
#include <memory>
#include <optional>
#include <string>

class Window final {
public:
	explicit Window( std::string_view title, uint32_t width, uint32_t height );
	~Window() noexcept;

	Window( const Window& ) = delete;
	Window( Window&& ) = delete;
	Window& operator=( const Window& ) = delete;
	Window& operator=( Window&& ) = delete;

	static std::optional<int> ProcessMessages() noexcept;
	HWND GetHandle() const noexcept { return m_hWnd; }
	Graphics& GetGfx() const noexcept { return *m_gfx; }

private:
	static LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	static LRESULT CALLBACK HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;
	LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept;

private:
	static constexpr const wchar_t* className = L"WindowClass";
	HWND m_hWnd = nullptr;
	std::unique_ptr<Graphics> m_gfx{ nullptr };

public:
	Input input;

};