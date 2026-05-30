#pragma once
#include "Draw/GeoUtils.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <cstdint>

class Graphics final {
public:
	explicit Graphics( HWND hWnd, uint32_t width, uint32_t height );
	~Graphics() = default;

	void PutPixel( Point position, Color color ) noexcept;
	void ClearScreen( Color color ) noexcept;

	int DrawLineDDA( Point p0, Point p1, Color color, float* outRuntimeMs = nullptr );
	int DrawLineBresenham( Point p0, Point p1, Color color, float* outRuntimeMs = nullptr );
	int DrawCircleMidpoint( Point center, int radius, Color color, float* outRuntimeMs = nullptr );
	int DrawEllipseMidpoint( Point center, int rx, int ry, Color color, float* outRuntimeMs = nullptr );

	void UpdateTexture();
	void EndFrame();
	void Present();


	ID3D11Device* GetDevice()  const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

private:
	Graphics( const Graphics& ) = delete;
	Graphics( Graphics&& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	Graphics& operator=( Graphics&& ) = delete;

	Microsoft::WRL::ComPtr<ID3D11Device>            m_device{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_context{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_pTexture{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         m_BackBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_renderTargetView{ nullptr };

	std::vector<uint32_t> m_frameBuffer{};
	uint32_t m_width{}, m_height{};
};