#include "DX11Renderer.h"
#include "Utility.h"

#ifdef _DEBUG
#include <comdef.h>
#define DX_CHECK(hr, msg) \
    if (FAILED(hr)) { \
        _com_error err(hr); \
        std::wstring fullMsg = std::wstring(msg) + L"\nError: " + err.ErrorMessage(); \
        MessageBox(NULL, fullMsg.c_str(), L"DirectX Debug Error", MB_ICONERROR); \
        __debugbreak(); \
    }
#else
#define DX_CHECK(hr, msg) hr 
#endif

DX11Renderer::DX11Renderer( HWND hwnd, int width, int height )
	: m_width( width ), m_height( height )
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{
		.BufferDesc = {
			.Width = static_cast<UINT>(width),
			.Height = static_cast<UINT>(height),
			.RefreshRate = {.Numerator = 0u, .Denominator = 1u },
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM
		},
		.SampleDesc = {.Count = 1u, .Quality = 0u },
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2u,
		.OutputWindow = hwnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Flags = 0u,
	};

	UINT createDeviceFlags = 0u;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	DX_CHECK(
		D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
			m_swapChain.GetAddressOf(), m_device.GetAddressOf(), nullptr, m_context.GetAddressOf()
		),
		L"Failed to create Device and SwapChain"
	);



	ComPtr<ID3D11Texture2D> pBackBuffer;
	DX_CHECK(
		m_swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) ),
		L"Failed to get SwapChain back buffer"
	);
	DX_CHECK(
		m_device->CreateRenderTargetView( pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf() ),
		L"Failed to create RenderTargetView"
	);



	auto vsData = LoadCSO( "VertexShader.cso" );
	DX_CHECK(
		m_device->CreateVertexShader( vsData.data(), vsData.size(), nullptr, &m_vertexShader ),
		L"Failed to create Vertex Shader"
	);

	auto psData = LoadCSO( "PixelShader.cso" );
	DX_CHECK(
		m_device->CreatePixelShader( psData.data(), psData.size(), nullptr, &m_pixelShader ),
		L"Failed to create Pixel Shader"
	);



	D3D11_TEXTURE2D_DESC canvasTextureDesc{
		.Width = static_cast<UINT>(width),
		.Height = static_cast<UINT>(height),
		.MipLevels = 1u,
		.ArraySize = 1u,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc = {.Count = 1u,.Quality = 0u },
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};
	DX_CHECK(
		m_device->CreateTexture2D( &canvasTextureDesc, nullptr, m_canvasTexture.GetAddressOf() ),
		L"Failed to create Canvas Texture"
	);



	DX_CHECK(
		m_device->CreateShaderResourceView( m_canvasTexture.Get(), nullptr, m_textureView.GetAddressOf() ),
		L"Failed to create Texture SRV"
	);



	D3D11_SAMPLER_DESC sampDesc{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
		.ComparisonFunc = D3D11_COMPARISON_NEVER
	};
	DX_CHECK(
		m_device->CreateSamplerState( &sampDesc, m_samplerState.GetAddressOf() ),
		L"Failed to create Sampler State"
	);
}

DX11Renderer::~DX11Renderer()
{
#ifdef _DEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if( SUCCEEDED( m_device.As( &d3dDebug ) ) )
	{
		d3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_DETAIL );
	}
#endif
}

void DX11Renderer::UpdateTexture( const std::vector<uint32_t>& pixels )
{
	// Validate pixel buffer size
	if( pixels.size() != static_cast<size_t>(m_width * m_height) )
	{
		throw std::runtime_error( "Pixel buffer size mismatch" );
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	if( SUCCEEDED( m_context->Map( m_canvasTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ) ) )
	{
		uint8_t* dest = static_cast<uint8_t*>(mapped.pData);
		const uint8_t* src = reinterpret_cast<const uint8_t*>(pixels.data());
		size_t rowBytes = m_width * sizeof( uint32_t );

		for( int y = 0; y < m_height; ++y )
		{
			memcpy( dest + (y * mapped.RowPitch), src + (y * rowBytes), rowBytes );
		}
		m_context->Unmap( m_canvasTexture.Get(), 0 );
	}
}

void DX11Renderer::BeginFrame()
{
	D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)m_width, (float)m_height, 0.0f, 1.0f };
	m_context->RSSetViewports( 1, &vp );

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_context->ClearRenderTargetView( m_renderTargetView.Get(), clearColor );

	m_context->OMSetRenderTargets( 1, m_renderTargetView.GetAddressOf(), nullptr );
}

void DX11Renderer::EndFrame()
{
	m_context->VSSetShader( m_vertexShader.Get(), nullptr, 0 );
	m_context->PSSetShader( m_pixelShader.Get(), nullptr, 0 );

	m_context->PSSetShaderResources( 0, 1, m_textureView.GetAddressOf() );
	m_context->PSSetSamplers( 0, 1, m_samplerState.GetAddressOf() );

	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->Draw( 3, 0 );

	m_swapChain->Present( 1, 0 );
}