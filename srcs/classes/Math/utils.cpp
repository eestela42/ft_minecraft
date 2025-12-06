#include <classes/Math/utils.hpp>

int mod_floor(int a, int n)
{
	return ((a % n) + n) % n;
}