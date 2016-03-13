#pragma once

#include <stdint.h>

#define NEW_COLOR(rr, gg, bb) { .r = { rr }, .g = { gg }, .b = { bb }, .a = { 0 } }
#define RAND_COLOR { .r = { rand() % 256 }, .g = { rand() % 256 }, .b = { rand() % 256 }, .a = { 0 } }

union Color_t
{
	uint32_t i;
	struct
	{
		uint8_t
			b,
			g,
			r,
			a;
	};
};

typedef union Color_t Color;

extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_WHITE;
extern const Color COLOR_BLACK;