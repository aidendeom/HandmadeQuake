#pragma once

#include <stdint.h>

struct Color_t
{
	uint8_t b,
			g,
			r,
			a;
};

typedef struct Color_t Color;

extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_WHITE;
extern const Color COLOR_BLACK;