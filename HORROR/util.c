#include "util.h"
#include <math.h>
#include <string.h>


int p_rand(int min, int max, int mul)
{
	static unsigned int index = 0;
	static unsigned int rand[] = {
		97, 59, 22, 52, 9, 75, 46, 6, 53, 18, 66, 34, 49, 24,
		88, 17, 64, 79, 92, 83, 56, 40, 28, 67, 35, 70, 12,
		32, 77, 7, 21, 44, 90, 51, 74, 98, 99, 58, 69, 87, 76,
		57, 0, 33, 1, 29, 84, 43, 95, 14, 73, 85, 96, 82, 91,
		11, 65, 3, 89, 50, 4, 26, 15, 72, 2, 27, 47, 10, 36, 31,
		78, 30, 25, 94, 45, 42, 48, 16, 54, 63, 86, 71, 19, 41,
		20, 23, 37, 5, 8, 55, 38, 93, 62, 60, 39, 68, 81, 61,
		80, 13
	};

	index = (index + 1) % 100;
	return (rand[index] % (max - min + 1) + min) * mul;
}

int p_approx_dist(float xf, float yf, float dxf, float dyf)
{
	int x, y, dx, dy;
	x = (int)xf; y = (int)yf; dx = (int)dxf; dy = (int)dyf;
	int d1 = (abs(x - dx) * 362) >> 8;
	int d2 = (abs(y - dy) * 362) >> 8;
	int k = abs(d1 - d2) >> 3;
	return ((d1 + d2) >> 2) + k;
}