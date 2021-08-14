#include <iostream>
#include <vector>

#include "shared_ptr.h"

using namespace another_smart_pointer;

class Base
{
public:

	virtual void report() const = 0;

	virtual ~Base() = default;
};

class Foo : public Base
{
public:

	virtual void report() const override
	{
		std::cout << "I am Foo and I am fine!" << std::endl;
	}
};

class Bear : public Base
{
public:

	virtual void report() const override
	{
		std::cout << "I am Bear and I am fine!" << std::endl;
	}
};

int main()
{
	std::vector< shared_ptr<Base> > objects;
	objects.push_back(shared_ptr<Base>(new Foo()));
	objects.push_back(shared_ptr<Base>(new Bear()));

	for (auto& obj : objects)
		(*obj).report();

    return 0;
}
