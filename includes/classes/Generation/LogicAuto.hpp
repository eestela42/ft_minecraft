#ifndef LOGICAUTO_HPP
# define LOGICAUTO_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <classes/Generation/DataHolder.hpp>
#include <classes/Generation/FunctionAuto.hpp>



// class t_schema
// {
// 		std::vector<int> func_order;
// 		std::vector<DataHolder> param;

// 		void do_stuff();// call func here ?
// };

class LogicAuto
{
	// private :
		DataHolder var;
		std::vector<FunctionAuto&> functions;
	public :
		LogicAuto();
		~LogicAuto();
		
		void do_stuff(DataHolder &inputs);
};





#endif