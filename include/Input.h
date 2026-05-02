#pragma once
#include "DSUtils.h"
#include <bitset>

struct Input {
	// Keyboard States
	std::bitset<256> keys;

	// Mouse States
	DSUtils::Point pos;

	int wheelDelta = 0;
	bool middleDown = false;

	bool leftDown = false;
	bool rightDown = false;

	// Helper Functions
	bool IsKeyPressed( unsigned char keycode ) const { return keys[keycode]; }
	void ClearWheel() { wheelDelta = 0; }
};