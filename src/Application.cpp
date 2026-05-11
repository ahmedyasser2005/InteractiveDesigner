#include "Application.h"
#include "CurveUtils.h"
#include "ErrorUtils.h"
#include <ShellScalingApi.h>
#include <imgui.h>
#include <format>
#include <cmath>

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
			case Tool::Circle:
				if( m_circleWaitingCenter )
				{
					m_circleCenter = clickPos;
					m_circleWaitingCenter = false;
					m_drawingCirclePreview = true;
					m_circleRadiusPreview = 0;
				}
				else
				{
					// Finalize the circle
					int radius = static_cast<int>(std::hypot( clickPos.x - m_circleCenter.x, clickPos.y - m_circleCenter.y ));
					m_circles.push_back( { m_circleCenter, radius, m_currentColor } );
					m_circleWaitingCenter = true;
					m_drawingCirclePreview = false;
				}
				break;
			case Tool::Ellipse:
				if( m_ellipseWaitingFirst )
				{
					m_ellipseCenter = clickPos;
					m_ellipseWaitingFirst = false;
					m_drawingEllipsePreview = true;
					m_ellipseRxPreview = 0;
					m_ellipseRyPreview = 0;
				}
				else
				{
					// Finalize: rx = |dx|, ry = |dy| from center to click
					int rx = static_cast<int>(std::abs( clickPos.x - m_ellipseCenter.x ));
					int ry = static_cast<int>(std::abs( clickPos.y - m_ellipseCenter.y ));
					m_ellipses.push_back( { m_ellipseCenter, rx, ry, m_currentColor } );
					m_ellipseWaitingFirst = true;
					m_drawingEllipsePreview = false;
				}
				break;
				// Future tools: Ellipse, etc.
		}
	}
	if( m_activeTool == Tool::BezierCurve )
	{
		// ---- Right-click: finish current curve (always) ----
		if( m_input->rightDown && !m_prevRightDown )
		{
			m_curveMode = false;
			m_currentCurveIndex = -1;
			m_selectedControlPoint = -1;
			m_selectedCurveForPoint = -1;
			m_draggingPoint = false;
		}

		// ---- Left-click handling ----
		if( leftClicked )
		{
			// Try to select/drag an existing control point (only when NOT adding points)
			if( !m_curveMode )
			{
				float minDist = 8.0f;  // pixel radius
				int hitCurve = -1, hitPoint = -1;
				for( size_t ci = 0; ci < m_curves.size(); ++ci )
				{
					const auto& curve = m_curves[ci];
					for( size_t pi = 0; pi < curve.controlPoints.size(); ++pi )
					{
						float dx = curve.controlPoints[pi].x - m_input->pos.x;
						float dy = curve.controlPoints[pi].y - m_input->pos.y;
						float dist = sqrtf( dx * dx + dy * dy );
						if( dist < minDist )
						{
							minDist = dist;
							hitCurve = (int)ci;
							hitPoint = (int)pi;
						}
					}
				}
				if( hitCurve >= 0 )
				{
					// Start dragging the selected point
					m_selectedCurveForPoint = hitCurve;
					m_selectedControlPoint = hitPoint;
					m_draggingPoint = true;
					// Do NOT add a new point or start a new curve
				}
				else
				{
					// No hit: start a new curve
					m_curves.push_back( BezierCurve{} );
					m_currentCurveIndex = (int)m_curves.size() - 1;
					m_curves.back().color = m_currentColor;
					m_curveMode = true;
					m_selectedControlPoint = -1;
					// Add first point
					if( m_currentCurveIndex >= 0 )
						m_curves[m_currentCurveIndex].controlPoints.push_back( m_input->pos );
				}
			}
			else // m_curveMode == true: we are adding points to the current curve
			{
				if( m_currentCurveIndex >= 0 )
					m_curves[m_currentCurveIndex].controlPoints.push_back( m_input->pos );
			}
		}

		// ---- Dragging update (every frame) ----
		if( m_draggingPoint && m_input->leftDown )
		{
			if( m_selectedCurveForPoint >= 0 && m_selectedControlPoint >= 0 )
			{
				m_curves[m_selectedCurveForPoint].controlPoints[m_selectedControlPoint] = m_input->pos;
			}
		}
		else
		{
			m_draggingPoint = false;
		}

		m_prevRightDown = m_input->rightDown;
	}
	if( m_activeTool == Tool::BezierCurve && !m_curveMode )
	{
		if( leftClicked && !m_draggingPoint )
		{
			// Check if we clicked on any control point (radius 5px)
			float minDist = 5.0f;
			m_selectedCurveForPoint = -1;
			m_selectedControlPoint = -1;
			for( size_t ci = 0; ci < m_curves.size(); ++ci )
			{
				auto& curve = m_curves[ci];
				for( size_t pi = 0; pi < curve.controlPoints.size(); ++pi )
				{
					float dx = curve.controlPoints[pi].x - m_input->pos.x;
					float dy = curve.controlPoints[pi].y - m_input->pos.y;
					float dist = sqrtf( dx * dx + dy * dy );
					if( dist < minDist )
					{
						minDist = dist;
						m_selectedCurveForPoint = (int)ci;
						m_selectedControlPoint = (int)pi;
					}
				}
			}
			if( m_selectedCurveForPoint >= 0 )
				m_draggingPoint = true;
		}

		// Dragging
		if( m_draggingPoint && (m_input->leftDown) )
		{
			if( m_selectedCurveForPoint >= 0 && m_selectedControlPoint >= 0 )
			{
				m_curves[m_selectedCurveForPoint].controlPoints[m_selectedControlPoint] = m_input->pos;
			}
		}
		else
		{
			m_draggingPoint = false;
		}
	}

	if( m_drawingPreview && !m_lineWaitingFirst )
	{
		m_previewEnd = m_input->pos;
	}
	if( m_drawingCirclePreview && !m_circleWaitingCenter )
	{
		Point current = m_input->pos;
		m_circleRadiusPreview = static_cast<int>(std::hypot( current.x - m_circleCenter.x, current.y - m_circleCenter.y ));
	}
	if( m_drawingEllipsePreview && !m_ellipseWaitingFirst )
	{
		Point current = m_input->pos;
		m_ellipseRxPreview = static_cast<int>(std::abs( current.x - m_ellipseCenter.x ));
		m_ellipseRyPreview = static_cast<int>(std::abs( current.y - m_ellipseCenter.y ));
	}


	if( m_animateDeCasteljau )
	{
		m_deCasteljauT += m_animationDir;
		if( m_deCasteljauT >= 1.0f ) { m_deCasteljauT = 1.0f; m_animationDir = -0.01f; }
		if( m_deCasteljauT <= 0.0f ) { m_deCasteljauT = 0.0f; m_animationDir = 0.01f; }
	}
}

void Application::Render()
{
	m_gfx->ClearScreen( { 30, 30, 30 } );



	for( const auto& line : m_lines )
		m_gfx->DrawLineBresenham( line.p0, line.p1, line.color );

	for( const auto& circle : m_circles )
		m_gfx->DrawCircleMidpoint( circle.center, circle.radius, circle.color );

	for( const auto& ellipse : m_ellipses )
		m_gfx->DrawEllipseMidpoint( ellipse.center, ellipse.rx, ellipse.ry, ellipse.color );

	// Draw all Bezier curves and their control points
	for( size_t ci = 0; ci < m_curves.size(); ++ci )
	{
		const auto& curve = m_curves[ci];
		drawBezierCurve( *m_gfx, curve.controlPoints, curve.color, 200 );

		// Draw control points as small circles
		for( size_t pi = 0; pi < curve.controlPoints.size(); ++pi )
		{
			Color ptColor = (m_selectedCurveForPoint == (int)ci && m_selectedControlPoint == (int)pi)
				? Color( 0, 255, 255 )   // cyan for selected
				: Color( 255, 255, 0 );  // yellow
			m_gfx->DrawCircleMidpoint( curve.controlPoints[pi], 4, ptColor );
		}
	}


	if( m_drawingPreview )
		m_gfx->DrawLineDDA( m_lineStart, m_previewEnd, m_currentColor );

	if( m_drawingCirclePreview && !m_circleWaitingCenter )
		m_lastCirclePixelCount = m_gfx->DrawCircleMidpoint( m_circleCenter, m_circleRadiusPreview, m_currentColor, &m_lastCircleTimeMs );

	if( m_drawingEllipsePreview && !m_ellipseWaitingFirst )
		m_lastEllipsePixelCount = m_gfx->DrawEllipseMidpoint( m_ellipseCenter, m_ellipseRxPreview, m_ellipseRyPreview, m_currentColor, &m_lastEllipseTimeMs );

	// De Casteljau visualization (if enabled and a valid curve is selected)
	if( m_showDeCasteljau && m_selectedCurveIndex >= 0 && m_selectedCurveIndex < (int)m_curves.size() )
	{
		const auto& curve = m_curves[m_selectedCurveIndex];
		const auto& points = curve.controlPoints;
		if( points.size() >= 2 )
		{
			auto levels = computeDeCasteljauLevels( points, m_deCasteljauT );

			// Colors: level 0 = yellow, deeper levels = orange/red
			Color levelColors[] = { {255,255,0}, {255,200,0}, {255,150,0}, {255,100,0}, {255,50,0} };

			for( size_t lvl = 0; lvl < levels.size(); ++lvl )
			{
				Color col = levelColors[std::min( lvl, sizeof( levelColors ) / sizeof( levelColors[0] ) - 1 )];
				const auto& pts = levels[lvl];

				// Draw lines between consecutive points at this level
				for( size_t i = 0; i + 1 < pts.size(); ++i )
					m_gfx->DrawLineBresenham( pts[i], pts[i + 1], col );

				// Draw small circles at each point
				for( const auto& pt : pts )
					m_gfx->DrawCircleMidpoint( pt, 3, col );
			}
			// The final point (curve point at t) is the only point in the last level
			if( !levels.empty() && !levels.back().empty() )
			{
				Point finalPoint = levels.back()[0];
				m_gfx->DrawCircleMidpoint( finalPoint, 5, { 255,255,255 } ); // highlight
			}
		}
	}
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
				m_ellipses.clear();

				m_lastCirclePixelCount = 0;
				m_lastCircleTimeMs = 0.0f;
				m_lastEllipsePixelCount = 0;
				m_lastEllipseTimeMs = 0.0f;

				m_curves.clear();
				m_curveMode = false;
				m_currentCurveIndex = -1;
				m_selectedCurveIndex = -1;
				m_showDeCasteljau = false;
				m_animateDeCasteljau = false;
				m_selectedCurveForPoint = -1;
				m_draggingPoint = false;

			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// ----- Left Toolbar -----
	ImGui::SetNextWindowPos( ImVec2( 0, menuHeight ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( toolbarWidth, display.y - menuHeight - statusHeight ), ImGuiCond_Always );
	ImGui::Begin( "Toolbar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );

	ImGui::Text( "Draw" );
	ImGui::Separator();

	if( ImGui::Button( "Line", ImVec2( 80, 35 ) ) )
	{
		m_activeTool = Tool::Line;
		m_lineWaitingFirst = true;
		m_drawingPreview = false;

		m_draggingPoint = false;
		m_selectedCurveForPoint = -1;
		m_selectedControlPoint = -1;
	}
	if( ImGui::Button( "Circle", ImVec2( 80, 35 ) ) )
	{
		m_activeTool = Tool::Circle;
		m_circleWaitingCenter = true;
		m_drawingCirclePreview = false;
		m_lineWaitingFirst = true;
		m_drawingPreview = false;

		m_draggingPoint = false;
		m_selectedCurveForPoint = -1;
		m_selectedControlPoint = -1;
	}
	if( ImGui::Button( "Ellipse", ImVec2( 80, 35 ) ) )
	{
		m_activeTool = Tool::Ellipse;
		m_ellipseWaitingFirst = true;
		m_drawingEllipsePreview = false;
		m_lineWaitingFirst = true;
		m_circleWaitingCenter = true;

		m_draggingPoint = false;
		m_selectedCurveForPoint = -1;
		m_selectedControlPoint = -1;
	}
	if( ImGui::Button( "Bezier", ImVec2( 80, 35 ) ) )
	{
		m_activeTool = Tool::BezierCurve;
		m_lineWaitingFirst = true;
		m_circleWaitingCenter = true;
		m_ellipseWaitingFirst = true;

		m_draggingPoint = false;
		m_selectedCurveForPoint = -1;
		m_selectedControlPoint = -1;
	}
	// Future buttons: idk, etc.

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
	if( m_activeTool == Tool::Circle )
	{
		ImGui::Text( "Circle Tool" );
		if( m_circleWaitingCenter )
			ImGui::Text( "Click to set center." );
		else
			ImGui::Text( "Click to set radius." );

		// --- Metrics for preview (Part A3 comparison) ---
		if( m_drawingCirclePreview && !m_circleWaitingCenter )
		{
			ImGui::Separator();
			ImGui::Text( "Preview Metrics:" );
			ImGui::Text( "  Pixels: %d", m_lastCirclePixelCount );
			ImGui::Text( "  Time: %.3f ms", m_lastCircleTimeMs );
		}
	}
	if( m_activeTool == Tool::Ellipse )
	{
		ImGui::Text( "Ellipse Tool" );
		if( m_ellipseWaitingFirst )
			ImGui::Text( "Click to set center." );
		else
			ImGui::Text( "Click to set rx (width) and ry (height)." );

		// --- Metrics for preview (Part A3 comparison) ---
		if( m_drawingEllipsePreview && !m_ellipseWaitingFirst )
		{
			ImGui::Separator();
			ImGui::Text( "Preview Metrics:" );
			ImGui::Text( "  Pixels: %d", m_lastEllipsePixelCount );
			ImGui::Text( "  Time: %.3f ms", m_lastEllipseTimeMs );
		}
	}
	if( m_activeTool == Tool::BezierCurve )
	{
		ImGui::Text( "Bezier Curve Tool" );
		if( m_curveMode && m_currentCurveIndex >= 0 && m_currentCurveIndex < (int)m_curves.size() )
			ImGui::Text( "Adding points: %zu so far. Right-click to finish.",
						m_curves[m_currentCurveIndex].controlPoints.size() );
		else
			ImGui::Text( "Click to start a new curve." );
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text( "Canvas" );
	if( ImGui::Button( "Clear All" ) )
	{
		m_lines.clear();
		m_circles.clear();
		m_ellipses.clear();

		m_lastCirclePixelCount = 0;
		m_lastCircleTimeMs = 0.0f;
		m_lastEllipsePixelCount = 0;
		m_lastEllipseTimeMs = 0.0f;

		m_curves.clear();
		m_curveMode = false;
		m_currentCurveIndex = -1;
		m_selectedCurveIndex = -1;
		m_showDeCasteljau = false;
		m_animateDeCasteljau = false;
		m_selectedCurveForPoint = -1;
		m_draggingPoint = false;
	}

	ImGui::Separator();
	ImGui::Text( "De Casteljau" );
	ImGui::Checkbox( "Show construction", &m_showDeCasteljau );
	if( m_showDeCasteljau )
	{
		if( ImGui::BeginCombo( "Curve", m_selectedCurveIndex >= 0 ?
							   std::to_string( m_selectedCurveIndex ).c_str() : "None" ) )
		{
			for( int i = 0; i < (int)m_curves.size(); ++i )
			{
				if( ImGui::Selectable( ("Curve " + std::to_string( i )).c_str(),
									   m_selectedCurveIndex == i ) )
					m_selectedCurveIndex = i;
			}
			ImGui::EndCombo();
		}
		ImGui::SliderFloat( "t (parameter)", &m_deCasteljauT, 0.0f, 1.0f );
		if( ImGui::Button( "Animate t" ) )
			m_animateDeCasteljau = !m_animateDeCasteljau;
		ImGui::SameLine();
		if( ImGui::Button( "Reset t" ) )
		{
			m_deCasteljauT = 0.5f;
			m_animateDeCasteljau = false;
		}
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
		case Tool::Circle: toolName = "Circle"; break;
		case Tool::Ellipse: toolName = "Ellipse"; break;
		case Tool::BezierCurve: toolName = "Bezier Curve (click points, right-click finish)"; break;
	}

	auto status = std::format( "Tool: {}  |  Mouse: ({:.0f}, {:.0f})  |  Items: {}",
							  toolName, m_input->pos.x, m_input->pos.y,
							  m_lines.size() + m_circles.size() + m_ellipses.size() );

	ImGui::TextUnformatted( status.c_str() );
	ImGui::End();
}