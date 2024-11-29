#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>


class AComponent
{
	public :
		AComponent();
		~AComponent();

	virtual void addComponent() = 0;
	virtual void removeComponent() = 0;
};

template <typename T>
class Component : public AComponent
{
	private :
		std::vector<T> values;
	
	public :
		Component();
		~Component();
		
		void addComponent(T value);
		void removeComponent(int index);
		
		T getComponent(int index);
		std::vector<T> getComponents();
};



#endif 