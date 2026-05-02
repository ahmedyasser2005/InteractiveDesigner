#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <vector>

class Graphics final {
public:
	explicit Graphics( HWND hWnd, uint32_t width, uint32_t height );
	~Graphics();

	void UpdateTexture();
	void PutPixel( uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b ) noexcept;
	void EndFrame();
	void ClearScreen( uint8_t r, uint8_t g, uint8_t b ) noexcept;

private:
	Graphics( const Graphics& ) = delete;
	Graphics( Graphics&& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	Graphics& operator=( Graphics&& ) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11Device>				m_device			{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_context			{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_swapChain			{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTexture			{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pTextureView		{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_BackBuffer		{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_renderTargetView	{ nullptr };

	std::vector<uint32_t> m_pixelBuffer{};
	uint32_t m_width{}, m_height{};
};