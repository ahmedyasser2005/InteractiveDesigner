#include "Application.h"
#include <ShellScalingApi.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

static constexpr const char* APP_NAME = "Interactive Designer App";
static constexpr uint32_t WIDTH = 1280U, HEIGHT = 720U;

int Application::Run()
{
	SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );

	m_input = std::make_unique<Input>();
	m_wnd = std::make_unique<Window>( APP_NAME, WIDTH, HEIGHT, m_input.get() );
	m_gfx = std::make_unique<Graphics>( m_wnd->GetHandle(), WIDTH, HEIGHT );

	while( true )
	{
		if( const auto exitCode = m_wnd->ProcessMessages() ) // if there is an exit code, the window has been closed.
		{
			Shutdown();
			return *exitCode;
		}

		Update();
		m_gfx->ClearScreen( { 0U, 0U, 0U } );
		Render();
		RenderUI();
		m_gfx->EndFrame();
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
	if( imGuiIO.WantCaptureMouse || imGuiIO.WantCaptureKeyboard ) return;






}

void Application::Render()
{




}

void Application::RenderUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - 150, 0 ) );
	ImGui::SetNextWindowSize( ImVec2( 150, ImGui::GetIO().DisplaySize.y ) );

	ImGui::Begin( "Toolbox", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

	ImGui::Text( "Primitives" );
	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Separator();

	ImGui::End();
	ImGui::Render();
}
