#include "Application.h"
#include <ShellScalingApi.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

int Application::Run()
{
	SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );
	m_wnd = std::make_unique<Window>( "Interactive Designer App", 1280U, 720U );

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
	ImGuiIO& imGuiIO = ImGui::GetIO();
	// ONLY handle app input if the mouse isn't over an ImGui window
	// AND ONLY handle keyboard if ImGui isn't typing in a text box.
	if( !imGuiIO.WantCaptureMouse && !imGuiIO.WantCaptureKeyboard )
	{
		if( m_wnd->input.leftDown )
		{
			m_wnd->GetGfx().PutPixel( m_wnd->input.pos, { 255U, 0U, 0U } );
		}

		if( m_wnd->input.IsKeyPressed( 'R' ) )
		{
			m_wnd->GetGfx().ClearScreen( { 0U, 0U, 0U } );
		}
	}

}

void Application::Render()
{

	// Start ImGui
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin( "Canvas Controls" );
		if( ImGui::Button( "Reset Canvas" ) )
		{
			m_wnd->GetGfx().ClearScreen( { 0U, 0U, 0U } );
		}
		ImGui::End();

		ImGui::Render();
	}
	// End ImGui

}
