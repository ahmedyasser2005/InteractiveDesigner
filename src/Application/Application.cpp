#include "Application.h"
#include <ShellScalingApi.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

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


	// Start ImGui
	{
		// Start ImGui Frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Define UI
		ImGui::Begin( "Toolbox" );
		static float color[3] = { 1.0f, 1.0f, 1.0f };
		ImGui::ColorEdit3( "Brush Color", color );
		if( ImGui::Button( "Clear Canvas" ) )
		{
			// logic to clear app state
		}
		ImGui::End();
		// Finalize ImGui( this creates vertex buffers for the UI )
		ImGui::Render();
	}
	// End ImGui

}
