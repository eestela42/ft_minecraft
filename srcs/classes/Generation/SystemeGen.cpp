#include <classes/Generation/SystemeGen.hpp>


SystemeGenerator::SystemeGenerator()
{
	//ecriture en brute
	FunctionAuto octave("type");

	SystemeCalculator *ocean = new SystemeCalculator();
	SystemeCalculator *continent = new SystemeCalculator();

	SystemeCall *caller = new SystemeCall();	
}

SystemeGenerator::~SystemeGenerator()
{
}
