#include "GUI.h"
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <stdexcept>

GUI::GUI( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context )
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();

	ImFont* font = io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\segoeui.ttf", 18.0f );
	if( !font )
	{
		io.Fonts->AddFontDefault();
	}

	// Scale the UI elements by roughly the same ratio (18 / 13 ≈ 1.38)
	ImGui::GetStyle().ScaleAllSizes( 1.38f );

	if( !ImGui_ImplWin32_Init( hWnd ) )
		throw std::runtime_error( "ImGui_ImplWin32_Init failed." );
	if( !ImGui_ImplDX11_Init( device, context ) )
		throw std::runtime_error( "ImGui_ImplDX11_Init failed." );
}

GUI::~GUI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GUI::NewFrame()
{
	// Important: Win32 then DX11, then NewFrame
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void GUI::Render()
{
	ImGui::Render();
	if( auto drawData = ImGui::GetDrawData() )
		ImGui_ImplDX11_RenderDrawData( drawData );
}