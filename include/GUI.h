#pragma once
#include <d3d11.h>
#include <imgui.h>

class GUI final {
public:
	GUI( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context );
	~GUI();

	void NewFrame();
	void Render();

private:
	GUI( const GUI& ) = delete;
	GUI& operator=( const GUI& ) = delete;
	GUI( GUI&& ) = delete;
	GUI& operator=( GUI&& ) = delete;
};