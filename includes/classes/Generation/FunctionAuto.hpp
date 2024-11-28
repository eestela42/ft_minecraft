#ifndef FUNCTIONAUTO_HPP
# define FUNCTIONAUTO_HPP

#include <classes/Generation/DataHolder.hpp>

class FunctionAuto
{
	//NEED TO TAKE INPUTS (VAR) AND OUTPUTS !!!!
	DataHolder var;
	std::string type;
	//fct_ptr

	FunctionAuto(std::string type);
	~FunctionAuto();

	void logic(DataHolder &inputs);
};

#endif