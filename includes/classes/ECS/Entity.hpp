#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <vector>
#include <bitset>


struct id
{
	int type;
	int value;
};

class Entity
{
	private :
		std::vector<id> ids;
		std::bitset<8>	flag_compo;
		std::bitset<8>	flag_info;

	public :
		Entity();
		~Entity();

		void 				addComponent(int type, int value);
		void 				removeComponent(int type);

		bool				hasComponent(int type);
		int 				getComponent(int type);

		std::vector<id> 	getComponents();
		std::bitset<8> 		getFlagCompo();
		std::bitset<8> 		getFlagInfo();
};



#endif