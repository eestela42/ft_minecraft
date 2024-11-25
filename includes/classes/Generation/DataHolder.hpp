#ifndef DATAHOLDER_HPP
# define DATAHOLDER_HPP

#include <unordered_map>

class DataHolder
{
	private :
		void *data;
		std::unordered_map<std::string, u_int> indices;		//indices des chaque variable
	
	public :
		
		DataHolder(u_int size);
		~DataHolder();
		
		void *get(std::string var_name);
		void add(void *value, u_int size);

};


#endif