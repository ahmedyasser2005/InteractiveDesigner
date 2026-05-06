#pragma once

struct Point {
	float x, y;

	// Constructors
	constexpr Point( float x = 0.0f, float y = 0.0f ) : x( x ), y( y ) {}

	// Point-Point Math
	Point operator+( const Point& rhs ) const noexcept { return { x + rhs.x, y + rhs.y }; }
	Point operator-( const Point& rhs ) const noexcept { return { x - rhs.x, y - rhs.y }; }

	void operator+=( const Point& rhs ) noexcept { x += rhs.x; y += rhs.y; }
	void operator-=( const Point& rhs ) noexcept { x -= rhs.x; y -= rhs.y; }

	// Point-Scalar Math
	Point operator+( float scalar ) const noexcept { return { x + scalar, y + scalar }; }
	Point operator-( float scalar ) const noexcept { return { x - scalar, y - scalar }; }
	Point operator*( float scalar ) const noexcept { return { x * scalar, y * scalar }; }
	Point operator/( float scalar ) const noexcept { return { x / scalar, y / scalar }; }

	void operator+=( float scalar ) noexcept { x += scalar; y += scalar; }
	void operator-=( float scalar ) noexcept { x -= scalar; y -= scalar; }
	void operator*=( float scalar ) noexcept { x *= scalar; y *= scalar; }
	void operator/=( float scalar ) noexcept { x /= scalar; y /= scalar; }

	// Comparison
	bool operator==( const Point& other ) const noexcept { return x == other.x && y == other.y; }
	bool operator!=( const Point& other ) const noexcept { return !(*this == other); }
};

struct Color {
	unsigned char r, g, b, a;

	// Constructors
	constexpr Color( unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255 ) : r( r ), g( g ), b( b ), a( a ) {}

	// Color-Color Math
	Color operator+( const Color& rhs ) const noexcept { return { unsigned char( r + rhs.r ), unsigned char( g + rhs.g ), unsigned char( b + rhs.b ), unsigned char( a + rhs.a ) }; }
	Color operator-( const Color& rhs ) const noexcept { return { unsigned char( r - rhs.r ), unsigned char( g - rhs.g ), unsigned char( b - rhs.b ), unsigned char( a - rhs.a ) }; }

	void operator+=( const Color& rhs ) noexcept { r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a; }
	void operator-=( const Color& rhs ) noexcept { r -= rhs.r; g -= rhs.g; b -= rhs.b; a -= rhs.a; }

	// Color-Scalar Math
	Color operator*( float scalar ) const noexcept { return { unsigned char( r * scalar ), unsigned char( g * scalar ), unsigned char( b * scalar ), unsigned char( a * scalar ) }; }
	Color operator/( float scalar ) const noexcept { return { unsigned char( r / scalar ), unsigned char( g / scalar ), unsigned char( b / scalar ), unsigned char( a / scalar ) }; }

	void operator*=( float scalar ) noexcept { r = unsigned char( r * scalar ); g = unsigned char( g * scalar ); b = unsigned char( b * scalar ); a = unsigned char( a * scalar ); }
	void operator/=( float scalar ) noexcept { r = unsigned char( r / scalar ); g = unsigned char( g / scalar ); b = unsigned char( b / scalar ); a = unsigned char( a / scalar ); }

	// Comparison
	bool operator==( const Color& other ) const noexcept { return r == other.r && g == other.g && b == other.b && a == other.a; }
	bool operator!=( const Color& other ) const noexcept { return !(*this == other); }
};

// Global Operators for Left-Hand Scalars
inline Point operator*( float scalar, const Point& p ) noexcept { return p * scalar; }
inline Color operator*( float scalar, const Color& c ) noexcept { return c * scalar; }

