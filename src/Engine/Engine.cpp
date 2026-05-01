#include "Engine.h"

Engine::Engine( std::wstring title, UINT width, UINT height )
{
	m_window = std::make_unique<Window>( width, height, title );
	m_renderer = std::make_unique<DX11Renderer>( m_window->GetHandle(), width, height );
	m_pixelBuffer = std::make_unique<std::vector<uint32_t>>( width * height, 0xFF000000 );
}

void Engine::Run()
{
	while( m_window->ProcessMessages() )
	{
		m_renderer->BeginFrame();

		Update();
		Render();

		m_renderer->UpdateTexture( *m_pixelBuffer );
		m_renderer->EndFrame();
	}
}


void Engine::Update()
{

}

void Engine::Render()
{

}
