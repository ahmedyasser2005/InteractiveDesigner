#pragma once
#include "GeoUtils.h"
#include "Platform/Graphics.h"
#include <vector>
#include <cmath>

// Compute binomial coefficient n choose k
int binomial( int n, int k )
{
	if( k < 0 || k > n ) return 0;
	int res = 1;
	for( int i = 1; i <= k; ++i )
		res = res * (n - k + i) / i;
	return res;
}

// Bernstein polynomial B_{i,n}(t)
float bernstein( int i, int n, float t )
{
	return binomial( n, i ) * powf( t, i ) * powf( 1 - t, n - i );
}

Point evaluateBezier( const std::vector<Point>& points, float t )
{
	if( points.empty() ) return { 0,0 };
	int n = (int)points.size() - 1;
	Point result{ 0,0 };
	for( int i = 0; i <= n; ++i )
	{
		float b = bernstein( i, n, t );
		result.x += points[i].x * b;
		result.y += points[i].y * b;
	}
	return result;
}

void drawBezierCurve( Graphics& gfx, const std::vector<Point>& points, Color color, int segments )
{
	if( points.size() < 2 ) return;
	for( int i = 0; i <= segments; ++i )
	{
		float t = (float)i / segments;
		Point p = evaluateBezier( points, t );
		if( i > 0 )
		{
			float tPrev = (float)(i - 1) / segments;
			Point pPrev = evaluateBezier( points, tPrev );
			gfx.DrawLineBresenham( pPrev, p, color );
		}
	}
}


// Returns a vector of vectors: each inner vector contains points at that interpolation level.
// level 0 = original control points, level 1 = first linear interpolation, etc.
static std::vector<std::vector<Point>> computeDeCasteljauLevels( const std::vector<Point>& points, float t )
{
	std::vector<std::vector<Point>> levels;
	if( points.empty() ) return levels;

	levels.push_back( points );
	std::vector<Point> current = points;
	while( current.size() > 1 )
	{
		std::vector<Point> next;
		for( size_t i = 0; i < current.size() - 1; ++i )
		{
			Point p = current[i] * (1.0f - t) + current[i + 1] * t;
			next.push_back( p );
		}
		levels.push_back( next );
		current = next;
	}
	return levels;
}




// Evaluate cubic B‑spline basis functions (uniform) for parameter s in [0,1]
inline void cubicBSplineBasis( float s, float& N0, float& N1, float& N2, float& N3 )
{
	float s2 = s * s;
	float s3 = s2 * s;
	N0 = (1.0f - s) * (1.0f - s) * (1.0f - s) / 6.0f;			// (1-s)^3 / 6
	N1 = (3.0f * s3 - 6.0f * s2 + 4.0f) / 6.0f;					// (3s^3 - 6s^2 + 4)/6
	N2 = (-3.0f * s3 + 3.0f * s2 + 3.0f * s + 1.0f) / 6.0f;		// (-3s^3 + 3s^2 + 3s + 1)/6
	N3 = s3 / 6.0f;                                             // s^3 / 6
}

// Evaluate a single segment of a uniform cubic B‑spline (control points P[segment] .. P[segment+3])
inline Point evaluateUniformCubicBSplineSegment( const std::vector<Point>& points, int segment, float s )
{
	if( segment < 0 || segment + 3 >= (int)points.size() )
		return points.back();
	const Point& P0 = points[segment];
	const Point& P1 = points[segment + 1];
	const Point& P2 = points[segment + 2];
	const Point& P3 = points[segment + 3];
	float N0, N1, N2, N3;
	cubicBSplineBasis( s, N0, N1, N2, N3 );
	return P0 * N0 + P1 * N1 + P2 * N2 + P3 * N3;
}

// Draw a uniform cubic B‑spline curve from the given control points.
// Requires at least 4 control points.
inline void drawBSplineCurve( Graphics& gfx, const std::vector<Point>& points, Color color, int segmentsPerSegment = 50 )
{
	if( points.size() < 4 ) return;

	int numSegments = (int)points.size() - 3;
	for( int seg = 0; seg < numSegments; ++seg )
	{
		Point prev = evaluateUniformCubicBSplineSegment( points, seg, 0.0f );
		for( int step = 1; step <= segmentsPerSegment; ++step )
		{
			float s = (float)step / segmentsPerSegment;
			Point cur = evaluateUniformCubicBSplineSegment( points, seg, s );
			gfx.DrawLineBresenham( prev, cur, color );
			prev = cur;
		}
	}
}