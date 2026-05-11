#pragma once
#include "GeoUtils.h"
#include "Graphics.h"
#include <vector>

// Compute binomial coefficient n choose k
int binomial( int n, int k );
// Bernstein polynomial B_{i,n}(t)
float bernstein( int i, int n, float t );

Point evaluateBezier( const std::vector<Point>& points, float t );

void drawBezierCurve( Graphics& gfx, const std::vector<Point>& points, Color color, int segments = 100 );