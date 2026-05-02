#include "Graphics.h"
#include "../Platform/ErrorUtils.h"

Graphics::Graphics( HWND hWnd, uint32_t width, uint32_t height ) : m_width( width ), m_height( height )
{
	DXGI_SWAP_CHAIN_DESC sd{};
	{
		sd.BufferDesc.Width = m_width;
		sd.BufferDesc.Height = m_height;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2;
		sd.OutputWindow = hWnd;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = 0;
	}

	UINT createDeviceFlags = 0;
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX_CALL( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
											createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION,
											&sd, &m_swapChain, &m_device, nullptr, &m_context ) );

	m_pixelBuffer.resize( m_width * m_height, 0 );

	D3D11_TEXTURE2D_DESC td{};
	{
		td.Width = m_width;
		td.Height = m_height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_DYNAMIC;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	DX_CALL( m_device->CreateTexture2D( &td, nullptr, &m_pTexture ) );
	DX_CALL( m_device->CreateShaderResourceView( m_pTexture.Get(), nullptr, &m_pTextureView ) );
	DX_CALL( m_swapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), &m_BackBuffer ) );
	DX_CALL( m_device->CreateRenderTargetView( m_BackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf() ) );
}

void Graphics::EndFrame()
{
	UpdateTexture();
	m_context->CopyResource( m_BackBuffer.Get(), m_pTexture.Get() );
	DX_CALL( m_swapChain->Present( 1, 0 ) );
}


void Graphics::UpdateTexture()
{
	D3D11_MAPPED_SUBRESOURCE msr;
	// Lock the GPU resource for writing
	HRESULT hr = m_context->Map( m_pTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr );


	if( SUCCEEDED( hr ) )
	{
		uint8_t* pDst = reinterpret_cast<uint8_t*>(msr.pData);
		uint8_t* pSrc = reinterpret_cast<uint8_t*>(m_pixelBuffer.data());

		// Copy row by row because GPU alignment (Pitch) might differ from Width
		for( UINT y = 0; y < m_height; y++ )
		{
			memcpy( pDst + (y * msr.RowPitch), pSrc + (y * m_width * sizeof( uint32_t )), m_width * sizeof( uint32_t ) );
		}

		m_context->Unmap( m_pTexture.Get(), 0 );
	}
}




void Graphics::PutPixel( uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b ) noexcept
{
	if( x >= m_width || y >= m_height ) return;
	m_pixelBuffer[y * m_width + x] = (255 << 24) | (r << 16) | (g << 8) | b;
}

void Graphics::ClearScreen( uint8_t r, uint8_t g, uint8_t b ) noexcept
{
	uint32_t color = (255 << 24) | (r << 16) | (g << 8) | b;
	std::fill( m_pixelBuffer.begin(), m_pixelBuffer.end(), color );
}