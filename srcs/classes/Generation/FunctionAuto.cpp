#include <classes/Generation/FunctionAuto.hpp>

FunctionAuto::FunctionAuto(std::string type)
{
	this->type = type;
	
	switch(1) //mettre un type
	{
		case 1 :
			//setup les variables
			//ex :
			break;
	}

}

FunctionAuto::~FunctionAuto()
{
	
}

void FunctionAuto::logic(DataHolder &inputs)
{
	switch(1) //mettre un type
	{
		case 1 :
			//call octave2d_classique avec les bons arguments
			//ex :
			int Pos = (int)*(inputs.get("Pos"));
			break;
	}
}





