#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <cstring>



class Component
{
	private :
		unsigned int id;
		unsigned int data_size;
		std::vector<unsigned char> values;
		std::vector<unsigned int> free_indices;
	
	public :
		Component(unsigned int id, unsigned int data_size);
		~Component();
		
		unsigned int addComponent(void* value);
		void removeComponent(unsigned int index);
		
		void* getComponent(unsigned int index);
		std::vector<unsigned char>* getComponents();

		unsigned int getId() {return id;};
};






#endif 