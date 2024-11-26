#ifndef SYSTEMEGEN_HPP
#define SYSTEMEGEN_HPP

#include <vector>
#include <classes/Generation/DataHolder.hpp>
#include <classes/Generation/LogicAuto.hpp>
#include <glm/glm.hpp>
#include <unordered_map>



class Merger; //  LogicAuto ?

class SystemeGenerator
{
	private :
		ASysteme *start;
	public :
		SystemeGenerator();
		~SystemeGenerator();
};

class ASysteme
{

	ASysteme();
	~ASysteme();

	virtual void do_stuff(DataHolder &data) = 0;
};

class SystemeCall : public ASysteme
{
	// private :
		std::vector<ASysteme&>	list_system;
		std::vector<Merger&> 	list_merger;

	public :
		SystemeCall();
		~SystemeCall();

		void do_stuff(DataHolder &data) override;
};

class SystemeCalculator : public ASysteme
{
	// private :
		DataHolder 	var;
		LogicAuto 	functions;

	public :
		SystemeCalculator();
		~SystemeCalculator();

		void do_stuff(DataHolder &data) override;
/*{
	logic.do_stuff	
	
}*/
};









#endif