#include <classes/ECS/Component.hpp>

AComponent::AComponent()
{
}

AComponent::~AComponent()
{
}

template <typename T>
Component<T>::Component()
{
}

template <typename T>
Component<T>::~Component()
{
}
template <typename T>
void Component<T>::addComponent(T value)
{
	values.push_back(value);	
}

template <typename T>
void Component<T>::removeComponent(int index)
{
	values.erase(values.begin() + index);
}

template <typename T>
T Component<T>::getComponent(int index)
{
	return values[index];
}

template <typename T>
std::vector<T> Component<T>::getComponents()
{
	return values;
}
