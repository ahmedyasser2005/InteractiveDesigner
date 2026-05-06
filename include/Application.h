#pragma once
#include "Window.h"
#include "Graphics.h"
#include "Input.h"
#include <memory>

class Application final {
public:
	int Run();

private:
	void Shutdown();
	void Update();
	void Render();
	void RenderUI();

private:
	std::unique_ptr<Window> m_wnd{ nullptr };
	std::unique_ptr<Graphics> m_gfx{ nullptr };
	std::unique_ptr<Input> m_input{ nullptr };
	
};