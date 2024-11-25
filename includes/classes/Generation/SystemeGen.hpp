#ifndef SYSTEMEGEN_HPP
#define SYSTEMEGEN_HPP

#include <vector>
#include <classes/Generation/DataHolder.hpp>
#include <glm/glm.hpp>
#include <unordered_map>



class Merger; //  LogicAuyo ?

class DataHolder
{
	private :
		std::vector<unsigned char> data; //"MAP???"
		//keep info about the data (pos, size, type, name...)
	public :
		void *get(std::string name);
};

typedef t_schema; //has the func call order + DataHolder info

class func_auto
{
	void logic(DataHolder &inputs);
/*{ exemple : 
	float pos_factor = inouts.get("pos_factor")
	int octave = inputs.get("octave");
	...
puis la fonction normale

	int height = 



}*/
};

class LogicAuto
{
	private :
		DataHolder var;
		std::vector<func_auto&> functions;
		t_schema schema;
	public :
		void do_stuff(DataHolder &inputs)
/*{

	while(schema)
	{
	DataHolder inputs = get_inputs(schema);
	func_auto.logic(inputs);

	schema.next;
	}
}*/

};


class SystemeGenerator
{
	ASysteme *start;
};

class ASysteme
{

	ASysteme();
	~ASysteme();

	virtual void do_stuff(DataHolder &data) = 0;
};

class SystemeCall : public ASysteme
{
	private :
		std::vector<ASysteme&>	list_system;
		std::vector<Merger&> 	list_merger;

	public :
		SystemeCall();
		~SystemeCall();

		void do_stuff(DataHolder &data) override;
};

class SystemeCalculator : public ASysteme
{
	private :
		DataHolder 	var;
		LogicAuto 	functions;

	public :
		void do_stuff(DataHolder &data) override;
/*{
	logic.do_stuff	
	
}*/
};









#endif