#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <vector>
#include <bitset>
#include <glm/glm.hpp>

struct Vec2Comparator {
    bool operator()(const glm::vec2& a, const glm::vec2& b) const {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }
};

struct id
{
	int type;
	int value;
};

class Entity
{
	private :
		std::vector<id> ids;
		std::bitset<8>	flag_compo; //size is 1 bit per component existing (to update)
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
		std::bitset<8>* 	getFlagInfoAddr();

};



#endif