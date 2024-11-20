#ifndef SYSTEMEGEN_HPP
#define SYSTEMEGEN_HPP

#include <vector>

class SystemeGen
{
private :
	std::vector<SystemeGen> list;
	std::vector<float> table;
	std::vector<float> mergers;

public:
	SystemeGen(std::vector<SystemeGen> list,
						std::vector<float> table,
						std::vector<float> mergers);
	~SystemeGen();

	virtual int getValue(float x, float y, float z);
};










#endif