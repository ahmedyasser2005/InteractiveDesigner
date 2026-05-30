#include "CustomWin.h"
#include "Graphics.h"
#include "Utility/ErrorUtils.h"
#include <chrono>
#include <cmath>

Graphics::Graphics( HWND hWnd, uint32_t width, uint32_t height )
	: m_width( width ), m_height( height )
{
	DXGI_SWAP_CHAIN_DESC sd{};
	{
		sd.BufferDesc.Width = m_width;
		sd.BufferDesc.Height = m_height;
		sd.BufferDesc.RefreshRate.Numerator = 0U;
		sd.BufferDesc.RefreshRate.Denominator = 0U;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = 1U;
		sd.SampleDesc.Quality = 0U;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2U;
		sd.OutputWindow = hWnd;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = 0U;
	}

	UINT createDeviceFlags = 0U;
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX_CALL( D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		createDeviceFlags, nullptr, 0U, D3D11_SDK_VERSION,
		&sd, &m_swapChain, &m_device, nullptr, &m_context ) );

	m_frameBuffer.resize( m_width * m_height, 0U );

	D3D11_TEXTURE2D_DESC td{};
	{
		td.Width = m_width;
		td.Height = m_height;
		td.MipLevels = 1U;
		td.ArraySize = 1U;
		td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		td.SampleDesc.Count = 1U;
		td.Usage = D3D11_USAGE_DYNAMIC;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	DX_CALL( m_device->CreateTexture2D( &td, nullptr, &m_pTexture ) );
	DX_CALL( m_device->CreateShaderResourceView( m_pTexture.Get(), nullptr, &m_pTextureView ) );
	DX_CALL( m_swapChain->GetBuffer( 0U, __uuidof(ID3D11Texture2D), &m_BackBuffer ) );
	DX_CALL( m_device->CreateRenderTargetView( m_BackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf() ) );
}

void Graphics::PutPixel( Point position, Color color ) noexcept
{
	const int ix = static_cast<int>(std::round( position.x ));
	const int iy = static_cast<int>(std::round( position.y ));

	if( ix < 0 || iy < 0 || ix >= static_cast<int>( m_width ) || iy >= static_cast<int>( m_height ) )
		return;

	m_frameBuffer[iy * m_width + ix] = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

void Graphics::ClearScreen( Color color ) noexcept
{
	const uint32_t hex = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	std::fill( m_frameBuffer.begin(), m_frameBuffer.end(), hex );
}

int Graphics::DrawLineDDA( Point p0, Point p1, Color color, float* outRuntimeMs )
{
	auto start = std::chrono::high_resolution_clock::now();

	const float dx = p1.x - p0.x;
	const float dy = p1.y - p0.y;
	const float steps = std::max( std::abs( dx ), std::abs( dy ) );

	// Degenerate case: start == end
	if( steps == 0.0f )
	{
		PutPixel( p0, color );
		if( outRuntimeMs ) *outRuntimeMs = 0.0f;
		return 1;
	}

	const float xInc = dx / steps;
	const float yInc = dy / steps;
	float x = p0.x, y = p0.y;
	int pixelCount = 0;

	for( int i = 0; i <= static_cast<int>(steps); ++i )
	{
		PutPixel( { x, y }, color );
		++pixelCount;
		x += xInc;
		y += yInc;
	}

	auto end = std::chrono::high_resolution_clock::now();
	if( outRuntimeMs )
		*outRuntimeMs = std::chrono::duration<float, std::milli>( end - start ).count();

	return pixelCount;
}

int Graphics::DrawLineBresenham( Point p0, Point p1, Color color, float* outRuntimeMs )
{
	auto start = std::chrono::high_resolution_clock::now();

	int x0 = static_cast<int>(std::round( p0.x ));
	int y0 = static_cast<int>(std::round( p0.y ));
	int x1 = static_cast<int>(std::round( p1.x ));
	int y1 = static_cast<int>(std::round( p1.y ));

	const int dx = std::abs( x1 - x0 );
	const int sx = (x0 < x1) ? 1 : -1;
	const int dy = -std::abs( y1 - y0 );
	const int sy = (y0 < y1) ? 1 : -1;
	int err = dx + dy;
	int pixelCount = 0;

	while( true )
	{
		PutPixel( { static_cast<float>( x0 ), static_cast<float>( y0 ) }, color );
		++pixelCount;

		if( x0 == x1 && y0 == y1 ) break;

		const int e2 = 2 * err;
		if( e2 >= dy ) { err += dy; x0 += sx; }
		if( e2 <= dx ) { err += dx; y0 += sy; }
	}

	auto end = std::chrono::high_resolution_clock::now();
	if( outRuntimeMs )
		*outRuntimeMs = std::chrono::duration<float, std::milli>( end - start ).count();

	return pixelCount;
}

int Graphics::DrawCircleMidpoint( Point center, int radius, Color color, float* outRuntimeMs )
{
	auto start = std::chrono::high_resolution_clock::now();

	if( radius <= 0 )
		return 0;

	int x = 0;
	int y = radius;
	int d = 1 - radius;          // initial decision parameter
	int pixelCount = 0;

	// Helper lambda to draw all 8 symmetric points
	auto drawSymmetric = [&]( int cx, int cy, int xo, int yo )
	{
		PutPixel( { static_cast<float>(cx + xo), static_cast<float>(cy + yo) }, color );
		PutPixel( { static_cast<float>(cx - xo), static_cast<float>(cy + yo) }, color );
		PutPixel( { static_cast<float>(cx + xo), static_cast<float>(cy - yo) }, color );
		PutPixel( { static_cast<float>(cx - xo), static_cast<float>(cy - yo) }, color );
		PutPixel( { static_cast<float>(cx + yo), static_cast<float>(cy + xo) }, color );
		PutPixel( { static_cast<float>(cx - yo), static_cast<float>(cy + xo) }, color );
		PutPixel( { static_cast<float>(cx + yo), static_cast<float>(cy - xo) }, color );
		PutPixel( { static_cast<float>(cx - yo), static_cast<float>(cy - xo) }, color );
		pixelCount += 8;
	};

	int cx = static_cast<int>(std::round( center.x ));
	int cy = static_cast<int>(std::round( center.y ));

	while( x <= y )
	{
		drawSymmetric( cx, cy, x, y );
		x++;
		if( d < 0 )
			d += 2 * x + 1;
		else
		{
			y--;
			d += 2 * (x - y) + 1;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	if( outRuntimeMs )
		*outRuntimeMs = std::chrono::duration<float, std::milli>( end - start ).count();

	return pixelCount;
}

int Graphics::DrawEllipseMidpoint( Point center, int rx, int ry, Color color, float* outRuntimeMs )
{
	auto start = std::chrono::high_resolution_clock::now();

	if( rx <= 0 || ry <= 0 )
		return 0;

	int cx = static_cast<int>(std::round( center.x ));
	int cy = static_cast<int>(std::round( center.y ));
	int x = 0;
	int y = ry;
	int pixelCount = 0;

	// Decision parameters
	long long rx2 = static_cast<long long>(rx) * rx;
	long long ry2 = static_cast<long long>(ry) * ry;
	long long twoRx2 = 2 * rx2;
	long long twoRy2 = 2 * ry2;

	long long p1 = ry2 - rx2 * ry + (rx2 / 4);   // initial decision parameter for region 1

	auto drawSymmetric = [&]( int xo, int yo )
	{
		PutPixel( { static_cast<float>(cx + xo), static_cast<float>(cy + yo) }, color );
		PutPixel( { static_cast<float>(cx - xo), static_cast<float>(cy + yo) }, color );
		PutPixel( { static_cast<float>(cx + xo), static_cast<float>(cy - yo) }, color );
		PutPixel( { static_cast<float>(cx - xo), static_cast<float>(cy - yo) }, color );
		pixelCount += 4;
	};

	// Region 1: slope magnitude < 1
	while( 2 * ry2 * x <= 2 * rx2 * y )
	{
		drawSymmetric( x, y );
		x++;
		if( p1 < 0 )
			p1 += twoRy2 * x + ry2;
		else
		{
			y--;
			p1 += twoRy2 * x - twoRx2 * y + ry2;
		}
	}

	// Region 2: slope magnitude > 1
	long long p2 = ry2 * static_cast<long long>( x + 0.5 ) * (x + 0.5)
		+ rx2 * static_cast<long long>(y - 1) * (y - 1)
		- rx2 * ry2;

	while( y >= 0 )
	{
		drawSymmetric( x, y );
		y--;
		if( p2 > 0 )
			p2 -= twoRx2 * y + rx2;
		else
		{
			x++;
			p2 += twoRy2 * x - twoRx2 * y + rx2;
		}
	}

	auto end = std::chrono::high_resolution_clock::now();
	if( outRuntimeMs )
		*outRuntimeMs = std::chrono::duration<float, std::milli>( end - start ).count();

	return pixelCount;
}

void Graphics::UpdateTexture()
{
	D3D11_MAPPED_SUBRESOURCE msr;
	HRESULT hr = m_context->Map( m_pTexture.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &msr );

	if( SUCCEEDED( hr ) )
	{
		uint8_t* pDst = reinterpret_cast<uint8_t*>(msr.pData);
		uint8_t* pSrc = reinterpret_cast<uint8_t*>(m_frameBuffer.data());

		for( uint32_t y = 0U; y < m_height; ++y )
		{
			memcpy( pDst + (y * msr.RowPitch),
				   pSrc + (y * m_width * sizeof( uint32_t )),
				   m_width * sizeof( uint32_t ) );
		}

		m_context->Unmap( m_pTexture.Get(), 0 );
	}
}

void Graphics::EndFrame()
{
	UpdateTexture();
	m_context->CopyResource( m_BackBuffer.Get(), m_pTexture.Get() );
	m_context->OMSetRenderTargets( 1U, m_renderTargetView.GetAddressOf(), nullptr );
}

void Graphics::Present()
{
	DX_CALL( m_swapChain->Present( 1U, 0U ) );
}