#include "Engine/Engine.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	Engine engine{ L"Interactive Designer", 1280u , 720u };
	engine.Run();
	return 0;
}