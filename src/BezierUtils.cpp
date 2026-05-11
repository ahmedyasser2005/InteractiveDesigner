#include "BezierUtils.h"

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