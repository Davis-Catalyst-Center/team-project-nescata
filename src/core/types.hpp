#pragma once

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

typedef union {
	struct {
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;
	};
	uint32 value;
} Color;