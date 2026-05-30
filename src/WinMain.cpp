#include "Application/Application.h"
#include "Utility/ErrorUtils.h"

int WINAPI wWinMain(
	[[maybe_unused]] _In_	  HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_	  PWSTR pCmdLine,
	[[maybe_unused]] _In_	  int nCmdShow
)
{
	try
	{
		return Application().Run();
	}
	catch( const std::exception& e )
	{
		ShowErrorMessageBoxA( "Fatal Error", e.what() );
		return -1;
	}
	catch( ... )
	{
		ShowErrorMessageBoxA( "Fatal Error", "Unknown exception caught in WinMain." );
		return -1;
	}
}