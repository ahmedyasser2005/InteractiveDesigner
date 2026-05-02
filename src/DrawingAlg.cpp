#include "DrawingAlg.h"
#include "Graphics.h"

void DrawingAlg::BasicLine( DSUtils::Point p0, DSUtils::Point p1 )
{
	float m = (p1.y - p0.y) / (p1.x - p0.x);
	float b = p0.y - m * p0.x;
	for( uint32_t x = (uint32_t)p0.x; x < (uint32_t)p1.x; ++x )
	{
		float y = m * x + b;
		//PutPixel( x, (int)(y + 0.5) );
	}
}
//void DDALine( Point p0, Point p1 );
//void BresenhamLine( Point p0, Point p1 );

//void RegularCircle( Point position, uint32_t radius );
//void BresenhamCircle( Point position, uint32_t radius );
