# Interactive Designer App

A lightweight 2D vector graphics application built from scratch in C++ using DirectX 11 and Dear ImGui. This project focuses on the manual implementation of fundamental rasterization and curve algorithms without the use of built-in graphics primitives.

---

## Features

### Raster Graphics
*   **Lines:** Choice of **DDA** or **Bresenham** algorithms. Includes real-time pixel count and performance metrics.
*   **Circles:** Midpoint Circle algorithm with 8-way symmetry.
*   **Ellipses:** Midpoint Ellipse algorithm using two-region calculation.

### Advanced Curves
*   **Bezier Curves:** Evaluation via Bernstein Basis polynomials. Features interactive control point manipulation (Yellow: Idle, Cyan: Selected).
*   **De Casteljau's Algorithm:** Visualized through dynamic construction lines, a $t$-parameter slider, and animation modes.
*   **B-Splines:** Uniform cubic B-splines ensuring $C^2$ continuity.

### Font & UI Tools
*   **Composite Bezier Letters:** Procedural generation of letters ‘S’ and ‘C’ using multiple Bezier segments.
*   **Interactive Workspace:** Includes a color picker, "Clear All" functionality, and a real-time status bar showing mouse coordinates and item counts.

---

## Requirements

*   **OS:** Windows 10/11
*   **Compiler:** Visual Studio 2019+ (C++ Desktop Development workload)
*   **Build System:** CMake 3.25+
*   **Graphics API:** DirectX 11 (Windows SDK)

### Third-Party Libraries
*   **Dear ImGui (Docking Branch):** Used for the graphical user interface.
*   *Note: All core drawing logic is custom-coded.*

---

## Building the Project

1. **Clone the Repository:**
	```bash
	git clone https://github.com/ahmedyasser2005/InteractiveDesigner.git
	cd InteractiveDesigner


2. Configure & Build:
The project is CMake-ready. You can open the root folder in Visual Studio or use the command line:
	```DOS
	mkdir build
	cd build
	cmake .. -A x64
	cmake --build . --config Release
	
3. **Run:**
Execute `build/Release/InteractiveDesigner.exe`.

> [!IMPORTANT]
> The application looks for `C:\Windows\Fonts\segoeui.ttf`. If the path differs on your system, the app will default to the standard ImGui font.

---

## How to Use

| Task | Action |
| :--- | :--- |
| **Drawing Primitives** | Select Tool (Line/Circle/Ellipse) -> Left-click start -> Drag -> Left-click end. |
| **Creating Curves** | Left-click to place control points. **Right-click** to finalize. |
| **Editing Curves** | Click and drag any existing control point to reshape the curve. |
| **Algorithm Comparison** | In the Line Properties panel, click **"Draw Test Lines"**. Red = DDA, Blue = Bresenham. |
| **Animation** | Select a Bezier curve, enable "De Casteljau" in the panel, and use the $t$-slider. |

---

## Algorithm Mapping

| Feature | Implementation | Source Location |
| :--- | :--- | :--- |
| **Line (DDA)** | Digital Differential Analyzer | `Graphics::DrawLineDDA` |
| **Line (Bresenham)** | Integer-only error adjustment | `Graphics::DrawLineBresenham` |
| **Circle** | Midpoint / Brensenham Circle | `Graphics::DrawCircleMidpoint` |
| **Ellipse** | Midpoint Two-Region | `Graphics::DrawEllipseMidpoint` |
| **Bezier** | Bernstein Polynomials | `CurveUtils.h` |
| **B-Spline** | Uniform Cubic Basis | `CurveUtils.h` |

---

## Performance Insights

The application tracks execution time in milliseconds (ms). 
*   **Bresenham vs. DDA:** You will notice Bresenham consistently outperforms DDA because it avoids floating-point arithmetic and rounding calls, making it ideal for low-level hardware optimizations.

---

## Known Limitations
*   **CPU Rendering:** The framebuffer is processed on the CPU; high object counts may impact FPS.
*   **Volatility:** No Save/Load functionality is currently implemented; progress is lost on exit.
*   **Hardcoded Assets:** The generated letters ‘S’ and ‘C’ are hardcoded and cannot be saved to a file.

---

## Future Enhancements
*   Undo/redo stack.
*   Export canvas to PNG/BMP.
*   Load/save control points to JSON or text file.
*   Drag-and-drop of primitive shapes.
*   Multi-curve selection and grouping.
