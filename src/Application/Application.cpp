#include "Application.h"
#include <ShellScalingApi.h>

int Application::Run()
{
	SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );
	m_wnd = std::make_unique<Window>( "Interactive Designer App", 1280, 720 );

	while( true )
	{
		if( const auto exitCode = m_wnd->ProcessMessages() ) // if there is an exit code, the window has been closed.
		{
			Shutdown();
			return *exitCode;
		}
		Update();
		Render();
		m_wnd->GetGfx().EndFrame();
	}
}

void Application::Shutdown()
{
	// Save application state, release resources, etc.
}



void Application::Update()
{



}

void Application::Render()
{
	m_wnd->GetGfx().ClearScreen( 255u, 255u, 255u );



}
