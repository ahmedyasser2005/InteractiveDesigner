#pragma once
#include "Platform/Window.h"
#include "Platform/Graphics.h"
#include "Platform/Input.h"
#include "EditorUI/GUI.h"
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
	struct ComparisonLine { Point p0, p1; Color color; int algorithm; }; // 0 = DDA, 1 = Bresenham
	struct Circle { Point center; int radius; Color color; };
	struct Ellipse { Point center; int rx, ry; Color color; };
	struct BezierCurve {
		std::vector<Point> controlPoints;
		Color color;
		bool isSelected = false;  // for future drag/drop
	};
	struct BSplineCurve {
		std::vector<Point> controlPoints;
		Color color;
		bool isSelected = false;
	};


	// Tool system
	enum class Tool { Line, Circle, Ellipse, BezierCurve, BSplineCurve };
	enum class LineAlgorithm { DDA, Bresenham };
	Tool m_activeTool = Tool::Line;
	LineAlgorithm m_currentLineAlgo = LineAlgorithm::Bresenham;


	bool m_prevLeftDown = false;
	bool m_prevRightDown = false;

	// Temporary state for line placement
	bool  m_lineWaitingFirst = true;
	Point m_lineStart;
	bool  m_drawingPreview = false;
	Point m_previewEnd;

	// Metrics for line algorithm
	int    m_lastLinePixelCount = 0;
	float  m_lastLineTimeMs = 0.0f;
	int    m_ddaPixelCount = 0;
	float  m_ddaTimeMs = 0.0f;
	int    m_bresenhamPixelCount = 0;
	float  m_bresenhamTimeMs = 0.0f;

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

	// Metrics for circle/ellipse preview
	int    m_lastCirclePixelCount = 0;
	float  m_lastCircleTimeMs = 0.0f;
	int    m_lastEllipsePixelCount = 0;
	float  m_lastEllipseTimeMs = 0.0f;

	// Temporary state for curve placement
	int m_currentCurveIndex = -1;
	int  m_selectedControlPoint = -1;
	bool m_curveMode = false;
	bool m_draggingPoint = false;

	// Temporary state for B‑spline placement
	int  m_currentBSplineIndex = -1;
	bool m_bsplineMode = false;
	int  m_selectedBSplinePoint = -1;
	int  m_selectedBSplineCurveForPoint = -1;
	bool m_draggingBSplinePoint = false;

	// De Casteljau visualization
	bool m_showDeCasteljau = false;
	float m_deCasteljauT = 0.5f;
	int m_selectedCurveIndex = -1;		// index into m_curves
	int m_selectedCurveForPoint = -1;   // index of curve whose control point is selected
	bool m_animateDeCasteljau = false;
	float m_animationDir = 0.01f;



	std::vector<Line>		 m_lines;
	std::vector<ComparisonLine> m_comparisonLines;
	std::vector<Circle>		 m_circles;
	std::vector<Ellipse>	 m_ellipses;
	std::vector<BezierCurve> m_curves;
	std::vector<BSplineCurve> m_bsplines;



	// Drawing color
	Color m_currentColor{ 255U, 0U, 0U, 255U };
	float m_colorEdit[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
};
