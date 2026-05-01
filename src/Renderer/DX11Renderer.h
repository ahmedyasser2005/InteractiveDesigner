#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

class DX11Renderer {
public:
	DX11Renderer( HWND hwnd, int width, int height );
	~DX11Renderer();

	void BeginFrame();
	void UpdateTexture( const std::vector<uint32_t>& pixels );
	void EndFrame();

private:
	ComPtr<IDXGISwapChain>			 m_swapChain;
	ComPtr<ID3D11Device>			 m_device;
	ComPtr<ID3D11DeviceContext>		 m_context;
	ComPtr<ID3D11RenderTargetView>	 m_renderTargetView;
	ComPtr<ID3D11Texture2D>			 m_canvasTexture;
	ComPtr<ID3D11ShaderResourceView> m_textureView;
	ComPtr<ID3D11VertexShader>		 m_vertexShader;
	ComPtr<ID3D11PixelShader>		 m_pixelShader;
	ComPtr<ID3D11SamplerState>		 m_samplerState;

	int m_width, m_height;
};