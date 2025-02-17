#include <classes/World/PerlinNoise.hpp>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

double PerlinNoise::fade(double t) { 
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) { 
	return a + t * (b - a); 
}

double PerlinNoise::grad(int hash, double x, double y, double z) {
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 gradient directions
	double u = h < 8 ? x : y,
		   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

		/*****	1 - constructors 		*****/

PerlinNoise::~PerlinNoise()
{}

PerlinNoise::PerlinNoise()
{
}

PerlinNoise::PerlinNoise(const PerlinNoise &copy)
{
	*this = copy;
}

PerlinNoise::PerlinNoise(unsigned int seed) {
	p.resize(size*size);

	// Fill p with values from 0 to 255
	std::iota(p.begin(), p.end(), 0);

	// Initialize a random engine with seed
	std::default_random_engine engine(seed);

	// Suffle  using the above random engine
	std::shuffle(p.begin(), p.end(), engine);

	// Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}


double PerlinNoise::Octave2D(double x, double y, const std::int32_t octaves, const double persistence) 
{
	double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0; 
    for(int i = 0; i < octaves; i++) {
        total += newNoise3d(x * frequency, y * frequency, 0.5) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;

}

double PerlinNoise::Octave3D(double x, double y, double z, const std::int32_t octaves, const double persistence) 
{
	double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;  
    for(int i = 0; i < octaves; i++) {
        total += newNoise3d(x * frequency, y * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2;
    }
    
    return total/maxValue;
}

double	PerlinNoise::newNoise3d(double x, double y, double z)
{
	// Find the unit cube that contains the point
	int X = (int) floor(x) & ((size*size)-1);
	int Y = (int) floor(y) & ((size*size)-1);
	int Z = (int) floor(z) & ((size*size)-1);

	// Find relative x, y,z of point in cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for each of x, y, z
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	// Hash coordinates of the 8 cube corners
	// std::cout << "A" << std::endl;
	// std::cout << "X " << X << " Y " << Y  << std::endl;
	// std::cout << "p.size " << p.size() << std::endl;
	// std::cout << "size " << size << std::endl;
	
	// printf("address = %p\n", &p);
	// printf("address = %p\n", p.data());

	// for (int i = 0; i < p.size(); i++)
	// {
	// 	std::cout << p[i] << std::endl;
	// }
	int A = p[X] + Y;

	int AA = p[A] + Z;
	
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	double res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x-1, y, z)), lerp(u, grad(p[AB], x, y-1, z), grad(p[BB], x-1, y-1, z))),	lerp(v, lerp(u, grad(p[AA+1], x, y, z-1), grad(p[BA+1], x-1, y, z-1)), lerp(u, grad(p[AB+1], x, y-1, z-1),	grad(p[BB+1], x-1, y-1, z-1))));
	// std::cout << "res " << res << std::endl;

	return (res + 1.0)/2.0;
}

		/*****	3 - data getters 		*****/


		/*****	4 - operators 		*****/

PerlinNoise &PerlinNoise::operator=(const PerlinNoise &copy)
{
	if (this != &copy)
	{
		*this = copy;
	}
	return (*this);
}