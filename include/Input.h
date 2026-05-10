#pragma once
#include "GeoUtils.h"
#include <bitset>

struct Input {
	// Keyboard state
	std::bitset<256> keys;

	// Mouse state
	Point pos;

	int  wheelDelta = 0;
	bool middleDown = false;
	bool leftDown = false;
	bool rightDown = false;

	// Helpers
	[[nodiscard]] bool IsKeyPressed( unsigned char keycode ) const noexcept { return keys[keycode]; }
	void ClearWheel() noexcept { wheelDelta = 0; }
};
