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

	// Persistent storage
	struct Line { Point p0, p1; Color color; };
	struct Circle { Point center; int radius; Color color; };
	struct Ellipse { Point center; int rx, ry; Color color; };
	struct BezierCurve {
		std::vector<Point> controlPoints;
		Color color;
		bool isSelected = false;  // for future drag/drop
	};

	// Tool system
	enum class Tool { Line, Circle, Ellipse, BezierCurve };
	Tool m_activeTool = Tool::Line;

	bool m_prevLeftDown = false;
	bool m_prevRightDown = false;

	// Temporary state for line placement
	bool  m_lineWaitingFirst = true;
	Point m_lineStart;
	bool  m_drawingPreview = false;
	Point m_previewEnd;

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

	// Temporary state for curve placement
	BezierCurve* m_currentCurve = nullptr;
	int  m_selectedControlPoint = -1;
	bool m_curveMode = false;
	bool m_draggingPoint = false;
	bool m_editingCurve = false;

	std::vector<Line>		 m_lines;
	std::vector<Circle>		 m_circles;
	std::vector<Ellipse>	 m_ellipses;
	std::vector<BezierCurve> m_curves;

	// Drawing color
	Color m_currentColor{ 255U, 0U, 0U, 255U };
	float m_colorEdit[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
};
