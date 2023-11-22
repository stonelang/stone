#include <stdio.h>

class Base {

public:
	virtual void Execute() { printf("base");}

};


class Child : public Base {

public:
	void Execute() override { 
		Base::Execute(); 
	printf("child");}
};

int main() {
	Child child;
	child.Execute();

	return 0;
}