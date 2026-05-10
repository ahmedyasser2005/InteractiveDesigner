#pragma once
#include <algorithm>
//#include <cmath>

struct Point {
	float x, y;

	constexpr Point( float x = 0.0f, float y = 0.0f ) : x( x ), y( y ) {}

	// Point-Point Math
	[[nodiscard]] Point operator+( const Point& rhs ) const noexcept { return { x + rhs.x, y + rhs.y }; }
	[[nodiscard]] Point operator-( const Point& rhs ) const noexcept { return { x - rhs.x, y - rhs.y }; }

	void operator+=( const Point& rhs ) noexcept { x += rhs.x; y += rhs.y; }
	void operator-=( const Point& rhs ) noexcept { x -= rhs.x; y -= rhs.y; }

	// Point-Scalar Math
	[[nodiscard]] Point operator+( float scalar ) const noexcept { return { x + scalar, y + scalar }; }
	[[nodiscard]] Point operator-( float scalar ) const noexcept { return { x - scalar, y - scalar }; }
	[[nodiscard]] Point operator*( float scalar ) const noexcept { return { x * scalar, y * scalar }; }
	[[nodiscard]] Point operator/( float scalar ) const noexcept
	{
		if( scalar == 0.0f )
			return { 0.0f, 0.0f };
		return { x / scalar, y / scalar };
	}

	void operator+=( float scalar ) noexcept { x += scalar; y += scalar; }
	void operator-=( float scalar ) noexcept { x -= scalar; y -= scalar; }
	void operator*=( float scalar ) noexcept { x *= scalar; y *= scalar; }
	void operator/=( float scalar ) noexcept { if( scalar != 0.0f ) { x /= scalar; y /= scalar; } }

	// Comparison (exact floating-point equality — acceptable for pixel coordinates)
	[[nodiscard]] bool operator==( const Point& other ) const noexcept { return x == other.x && y == other.y; }
	[[nodiscard]] bool operator!=( const Point& other ) const noexcept { return !(*this == other); }
};

struct Color {
	unsigned char r, g, b, a;

	constexpr Color( unsigned char r = 0, unsigned char g = 0,
					unsigned char b = 0, unsigned char a = 255 )
		: r( r ), g( g ), b( b ), a( a )
	{}

	// Color-Color Math
	[[nodiscard]] Color operator+( const Color& rhs ) const noexcept
	{
		return { ClampToByte( r + rhs.r ), ClampToByte( g + rhs.g ),
				 ClampToByte( b + rhs.b ), ClampToByte( a + rhs.a ) };
	}
	[[nodiscard]] Color operator-( const Color& rhs ) const noexcept
	{
		return { ClampToByte( r - rhs.r ), ClampToByte( g - rhs.g ),
				 ClampToByte( b - rhs.b ), ClampToByte( a - rhs.a ) };
	}

	void operator+=( const Color& rhs ) noexcept { *this = *this + rhs; }
	void operator-=( const Color& rhs ) noexcept { *this = *this - rhs; }

	// Color-Scalar Math
	[[nodiscard]] Color operator*( float scalar ) const noexcept
	{
		return { ClampToByte( static_cast<int>(r * scalar) ),
				 ClampToByte( static_cast<int>(g * scalar) ),
				 ClampToByte( static_cast<int>(b * scalar) ),
				 ClampToByte( static_cast<int>(a * scalar) ) };
	}
	[[nodiscard]] Color operator/( float scalar ) const noexcept
	{
		if( scalar == 0.0f ) return { 0, 0, 0, 0 };
		return { ClampToByte( static_cast<int>(r / scalar) ),
				 ClampToByte( static_cast<int>(g / scalar) ),
				 ClampToByte( static_cast<int>(b / scalar) ),
				 ClampToByte( static_cast<int>(a / scalar) ) };
	}

	void operator*=( float scalar ) noexcept { *this = *this * scalar; }
	void operator/=( float scalar ) noexcept { *this = *this / scalar; }

	// Comparison
	[[nodiscard]] bool operator==( const Color& other ) const noexcept
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}
	[[nodiscard]] bool operator!=( const Color& other ) const noexcept
	{
		return !(*this == other);
	}

private:
	static constexpr unsigned char ClampToByte( int value )
	{
		return static_cast<unsigned char>(std::clamp( value, 0, 255 ));
	}
};

// Scalar * Value (for left-hand scalar multiplication)
[[nodiscard]] inline Point operator*( float scalar, const Point& p ) noexcept { return p * scalar; }
[[nodiscard]] inline Color operator*( float scalar, const Color& c ) noexcept { return c * scalar; }