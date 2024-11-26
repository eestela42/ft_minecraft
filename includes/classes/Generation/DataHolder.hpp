#ifndef DATAHOLDER_HPP
# define DATAHOLDER_HPP

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>

class DataHolder
{	//challenge = recuper le bon type
	private :
		std::vector<unsigned char> data;
		std::unordered_map<std::string, u_int> indice;		//indices des chaque variable
	
	public :
		
		DataHolder();
		~DataHolder();
		
		void *get(std::string var_name);//return a ptr to the address of the var
		void add(std::string var_name, unsigned char *value, u_int size);

};


#endif