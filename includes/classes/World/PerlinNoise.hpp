#ifndef PERLINNOISE_HPP
#define PERLINNOISE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

class PerlinNoise
{

private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);

	std::vector<int> p;
	const u_int size = 16; // not working, need to adapt in grad() and maybe other fct
public:
	/*****	1 - constructors 		*****/

	~PerlinNoise();
	PerlinNoise();
	PerlinNoise(unsigned int seed);
	PerlinNoise(const PerlinNoise &copy);

	/*****	2 - noise makers	 	*****/

	void noise1d(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput);
	void noise2d(int nWidth, int nheight, float *fSeed, int nOctaves, float fBias, float *fOutput);

	double Octave2D(double x, double y, const std::int32_t octaves, const double persistence);
	double Octave3D(double x, double y, double z, const std::int32_t octaves, const double persistence);

	double newNoise3d(double x, double y, double z);
	// double							noise(double x, double y, double z);

	/*****	3 - data getters 		*****/

	/*****	4 - operators 			*****/

	PerlinNoise &operator=(const PerlinNoise &copy);
};

#endif