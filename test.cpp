#include <iostream>
#include "BML.h"

int main()
{
	auto root = std::make_shared<bml::ValueNode>("root");
	auto child1 = std::make_shared<bml::ValueNode>("a", 1);
	auto child2 = std::make_shared<bml::ValueNode>("b", 2);
	auto child3 = std::make_shared<bml::ValueNode>("c", 3);
	auto child4 = std::make_shared<bml::ValueNode>("d", 4);
	
	root->AddChild(child1);
	child1->AddChild(child2);
	child2->AddChild(child3);
	child3->AddChild(child4);
	bml::Serialize::ToTextFile(root, "1.txt");
	auto read = bml::Serialize::FromTextFile("1.txt");
	return 0;
}