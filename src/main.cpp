#include "Application/Application.h"

int WINAPI wWinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ PWSTR pCmdLine,
	[[maybe_unused]] _In_ int nCmdShow
)
{
	Application app;
	return app.Run();
}