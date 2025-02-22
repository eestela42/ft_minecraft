#include <classes/ECS/Component.hpp>
#include <iostream>
#include <glm/glm.hpp>

Component::Component(unsigned int id, unsigned int data_size) : id(id), data_size(data_size)
{//should reserve for better starting perf
}


Component::~Component()
{
}

unsigned int Component::addComponent(void* value)
{
	// if (free_indices.size() > 0 && false) //not implemented yet
	// {
	// 	//it is bad to erase the first element of a vector
	// 	unsigned int index = free_indices[(free_indices.size() - 1)];
		
    //     std::memcpy(values.data() + index * data_size, value, data_size);
	// 	free_indices.erase(free_indices.end() - 1);
	// 	return index;
	// }

	if (values.capacity() - values.size() < data_size)
		values.reserve(values.capacity() + data_size); // a faire mieux


	values.resize(values.size() + data_size);

	std::memcpy(values.data() + values.size() - data_size, value, data_size);

	return values.size() - data_size;
}


void Component::removeComponent(unsigned int index)
{
	// std::memcpy(values.data() + index * data_size, NULL, data_size); //zeroed
	free_indices.push_back(index);
}


void* Component::getComponent(unsigned int index)
{
	return &(values[index]);
}


std::vector<unsigned char>* Component::getComponents()
{
	return &values;
}

void Component::print()
{
	std::cout << "Component : " << id << std::endl;
	for (int i = 0; i < values.size() / data_size; i += data_size)
	{
		std::cout << "Entity : " << i/data_size << std::endl;
		std::cout << "Value : " << ((glm::vec3)((values[i]))).x << " " << ((glm::vec3)((values[i]))).y << " " << ((glm::vec3)((values[i]))).z << " " << std::endl;
	}
}
