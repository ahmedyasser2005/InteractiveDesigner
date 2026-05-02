#pragma once
#include "Platform/Window.h"
#include <memory>


class Application final {
public:
	int Run();

private:
	void Shutdown();
	void Update();
	void Render();

private:
	std::unique_ptr<Window> m_wnd{ nullptr };

};