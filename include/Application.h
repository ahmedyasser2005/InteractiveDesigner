#pragma once
#include "Window.h"
#include "Graphics.h"
#include "Input.h"
#include "GUI.h"
#include <memory>
#include <vector>

class Application final {
public:
	int Run();

private:
	void Update();
	void Render();
	void RenderUI();

	std::unique_ptr<Window>   m_wnd;
	std::unique_ptr<Graphics> m_gfx;
	std::unique_ptr<Input>    m_input;
	std::unique_ptr<GUI>      m_gui;

	// Tool system
	enum class Tool { Line, Circle, Ellipse };
	Tool m_activeTool = Tool::Line;

	// Temporary state for line placement
	bool  m_lineWaitingFirst = true;
	Point m_lineStart;
	bool  m_drawingPreview = false;
	Point m_previewEnd;
	bool  m_prevLeftDown = false;

	// Temporary state for circle placement
	bool  m_circleWaitingCenter = true;   // true = waiting for center, false = waiting for radius
	Point m_circleCenter;
	int   m_circleRadiusPreview = 0;
	bool  m_drawingCirclePreview = false;

	// Temporary state for ellipse placement
	bool  m_ellipseWaitingFirst = true;   // true = waiting center, false = waiting rx/ry
	Point m_ellipseCenter;
	int   m_ellipseRxPreview = 0;
	int   m_ellipseRyPreview = 0;
	bool  m_drawingEllipsePreview = false;


	// Persistent storage
	struct Line { Point p0, p1; Color color; };
	struct Circle { Point center; int radius; Color color; };
	struct Ellipse { Point center; int rx, ry; Color color; };

	std::vector<Line>    m_lines;
	std::vector<Circle>  m_circles;
	std::vector<Ellipse> m_ellipses;

	// Drawing color
	Color m_currentColor{ 255U, 0U, 0U, 255U };
	float m_colorEdit[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
};
