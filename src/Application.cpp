#include "Application.h"
#include "ErrorUtils.h"
#include <ShellScalingApi.h>
#include <imgui.h>
#include <format>

static constexpr const char* APP_NAME = "Interactive Designer App";
static constexpr uint32_t WIDTH = 1280U, HEIGHT = 720U;

int Application::Run()
{
	// Enable per-monitor DPI awareness; fallback to system DPI if unavailable
	if( FAILED( SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE ) ) )
		SetProcessDpiAwareness( PROCESS_SYSTEM_DPI_AWARE );

	m_input = std::make_unique<Input>();
	m_wnd = std::make_unique<Window>( APP_NAME, WIDTH, HEIGHT, m_input.get() );
	m_gfx = std::make_unique<Graphics>( m_wnd->GetHandle(), WIDTH, HEIGHT );
	m_gui = std::make_unique<GUI>( m_wnd->GetHandle(), m_gfx->GetDevice(), m_gfx->GetContext() );

	try
	{
		while( true )
		{
			if( const auto exitCode = m_wnd->ProcessMessages() )
				return *exitCode;

			Update();
			Render();

			m_gfx->EndFrame();

			m_gui->NewFrame();
			RenderUI();
			m_gui->Render();

			m_gfx->Present();
		}
	}
	catch( const std::exception& e )
	{
		ShowErrorMessageBoxA( "Fatal Error", e.what() );
		return -1;
	}
}

void Application::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	if( io.WantCaptureMouse )
		return;

	const bool leftDown = m_input->leftDown;
	const bool leftClicked = leftDown && !m_prevLeftDown;
	m_prevLeftDown = leftDown;

	if( leftClicked )
	{
		const Point clickPos = m_input->pos;

		switch( m_activeTool )
		{
			case Tool::Line:
				if( m_lineWaitingFirst )
				{
					m_lineStart = clickPos;
					m_lineWaitingFirst = false;
					m_drawingPreview = true;
					m_previewEnd = clickPos;
				}
				else
				{
					m_lines.push_back( { m_lineStart, clickPos, m_currentColor } );
					m_lineWaitingFirst = true;
					m_drawingPreview = false;
				}
				break;

				// Future tools: Circle, Ellipse, etc.
		}
	}

	if( m_drawingPreview && !m_lineWaitingFirst )
		m_previewEnd = m_input->pos;
}

void Application::Render()
{
	m_gfx->ClearScreen( { 30, 30, 30 } );

	for( const auto& line : m_lines )
		m_gfx->DrawLineBresenham( line.p0, line.p1, line.color );

	if( m_drawingPreview )
		m_gfx->DrawLineDDA( m_lineStart, m_previewEnd, m_currentColor );
}

void Application::RenderUI()
{
	const ImVec2 display = ImGui::GetIO().DisplaySize;
	const float  menuHeight = 20.0f;
	const float  statusHeight = 25.0f;
	const float  toolbarWidth = 100.0f;
	const float  panelWidth = 250.0f;

	// ----- Top Menu Bar -----
	if( ImGui::BeginMainMenuBar() )
	{
		if( ImGui::BeginMenu( "File" ) )
		{
			if( ImGui::MenuItem( "Exit", "Alt+F4" ) )
				PostQuitMessage( 0 );
			ImGui::EndMenu();
		}
		if( ImGui::BeginMenu( "Edit" ) )
		{
			if( ImGui::MenuItem( "Clear All" ) )
			{
				m_lines.clear();
				m_circles.clear();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// ----- Left Toolbar -----
	ImGui::SetNextWindowPos( ImVec2( 0, menuHeight ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( toolbarWidth, display.y - menuHeight - statusHeight ), ImGuiCond_Always );
	ImGui::Begin( "Toolbar", nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );

	ImGui::Text( "Draw" );
	ImGui::Separator();

	if( ImGui::Button( "Line", ImVec2( 80, 35 ) ) )
	{
		m_activeTool = Tool::Line;
		m_lineWaitingFirst = true;
		m_drawingPreview = false;
	}
	// Future buttons: Circle, Ellipse, Bezier, etc.

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text( "Color" );

	if( ImGui::ColorButton( "CurrColor",
							ImVec4( m_colorEdit[0], m_colorEdit[1], m_colorEdit[2], m_colorEdit[3] ) ) )
		ImGui::OpenPopup( "ColorPicker" );

	if( ImGui::BeginPopup( "ColorPicker" ) )
	{
		ImGui::ColorPicker4( "##picker", m_colorEdit );
		m_currentColor = {
			static_cast<unsigned char>(m_colorEdit[0] * 255.0f),
			static_cast<unsigned char>(m_colorEdit[1] * 255.0f),
			static_cast<unsigned char>(m_colorEdit[2] * 255.0f),
			static_cast<unsigned char>(m_colorEdit[3] * 255.0f)
		};
		ImGui::EndPopup();
	}
	ImGui::End();

	// ----- Right Properties Panel -----
	ImGui::SetNextWindowPos( ImVec2( display.x - panelWidth, menuHeight ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( panelWidth, display.y - menuHeight - statusHeight ), ImGuiCond_Always );
	ImGui::Begin( "Properties", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

	ImGui::Text( "Tool Options" );
	ImGui::Separator();

	if( m_activeTool == Tool::Line )
	{
		ImGui::Text( "Line Tool" );
		ImGui::Text( "Click two points on canvas." );
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text( "Canvas" );
	if( ImGui::Button( "Clear All" ) )
	{
		m_lines.clear();
		m_circles.clear();
	}
	ImGui::End();

	// ----- Bottom Status Bar -----
	ImGui::SetNextWindowPos( ImVec2( 0, display.y - statusHeight ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( display.x, statusHeight ), ImGuiCond_Always );
	ImGui::Begin( "StatusBar", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs );

	const char* toolName = "Unknown";
	switch( m_activeTool )
	{
		case Tool::Line: toolName = "Line"; break;
	}

	auto status = std::format( "Tool: {}  |  Mouse: ({:.0f}, {:.0f})  |  Items: {}",
							  toolName, m_input->pos.x, m_input->pos.y,
							  m_lines.size() + m_circles.size() );
	ImGui::TextUnformatted( status.c_str() );
	ImGui::End();
}