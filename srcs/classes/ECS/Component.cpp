#include <classes/ECS/Component.hpp>
#include <iostream>
#include <glm/glm.hpp>

Component::Component(unsigned int id, unsigned int data_size) : id(id), data_size(data_size)
{
}


Component::~Component()
{
}

unsigned int Component::addComponent(void* value)
{
	if (free_indices.size() > 0)
	{
		//it is bad to erase the first element of a vector
		unsigned int index = free_indices[(free_indices.size() - 1)];
		
        std::memcpy(values.data() + index * data_size, value, data_size);
		free_indices.erase(free_indices.end() - 1);
		return index;
	}
	if (values.capacity() - values.size() < data_size)
	{
		values.reserve(values.capacity() + data_size); // a faire mieux
	}


	// std::cout << "pos mecpy " << values.data() + values.size() << std::endl;
	std::memcpy(values.data() + values.size(), value, data_size);
	values.resize(values.size() + data_size);

	// std::cout << "value.size" << values.size() << std::endl;
	return values.size() - 1;
}


void Component::removeComponent(unsigned int index)
{
	std::memcpy(values.data() + index * data_size, NULL, data_size); //zeroed
	free_indices.push_back(index);
}


void* Component::getComponent(unsigned int index)
{
	printf("in getCompo data[0] %p\n", &(values[index * data_size]));
	glm::vec3* pos = (glm::vec3*)&(values[index * data_size]);
	// std::cout << "pos x: " << pos->x << std::endl;

	return &(values[index * data_size]);
}


std::vector<unsigned char>* Component::getComponents()
{
	return &values;
}
