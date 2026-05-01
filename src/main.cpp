#include "Platform/Window.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	Window window( 1280, 720, L"Interactive Designer" );

	while( window.ProcessMessages() )
	{
		// Engine logic, Rasterizer, and DX11 rendering go here!
	}

	return 0;
}