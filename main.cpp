#include "json.hpp"
#include <iostream>

int main(int ac, char* av[])
{
	if (ac < 3)
	{
		std::cout << av[0] << " <in.json> <out.json>\n";
		return 1;
	}

	auto doc = Json::Value::FromFile(av[1]);
	doc.WriteToFile(av[2]);

	return 0;
}
