#ifndef SYSTEMEGEN_HPP
#define SYSTEMEGEN_HPP

#include <vector>

class SystemeGroundGen
{
private :
	std::vector<SystemeGroundGen> list;
	std::vector<float> table;
	std::vector<float> mergers;

public:
	SystemeGroundGen(std::vector<SystemeGroundGen> list,
						std::vector<float> table,
						std::vector<float> mergers);
	~SystemeGroundGen();

	virtual int getValue(float x, float y, float z);
};

// class SystemeBlocksGen
// {
// private :
// 	std::vector<SystemeBlocksGen> list;
// 	std::vector<float> table;
// 	std::vector<float> mergers;
// };









#endif