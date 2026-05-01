#pragma once
#include "Platform/Window.h"
#include "Renderer/DX11Renderer.h"
#include <memory>

class Engine {
public:
	Engine( std::wstring title, UINT width, UINT height );
	~Engine() = default;

	void Run();

private:
	void Update();
	void Render();

	std::unique_ptr<Window> m_window{ nullptr };
	std::unique_ptr<DX11Renderer> m_renderer{ nullptr };
	std::unique_ptr<std::vector<uint32_t>> m_pixelBuffer{ nullptr };
};