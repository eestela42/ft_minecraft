#include <classes/Generation/DataHolder.hpp>

DataHolder::DataHolder()
{}

DataHolder::~DataHolder()
{}

void*	DataHolder::get(std::string var_name)
{
	return((void*)&(data[indice[var_name]]));
}

void DataHolder::add(std::string var_name, unsigned char *value, u_int size)
{
	if (indice.find(var_name) != indice.end())
	{
		std::cout << "ERROR var name = " << var_name << std::endl;
		assert(!"Var name already in use\n");
	}
	data.reserve(data.size() + size);
	for (int i = 0; i < size; i++)
	{
		data.push_back(value[i]);
	}
}