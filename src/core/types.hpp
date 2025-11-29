#pragma once

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;


union Color {
	struct {
		uint8 r : 8;
		uint8 g : 8;
		uint8 b : 8;
		uint8 a : 8;
	};
	uint32 value = 0xFFFFFFFF;
};