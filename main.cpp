#include <iostream>

#include "ping.hpp"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "ip address!" << std::endl;
		return 1;
	}
	Ping* test = new Ping(argv[1]);
	test->run();
	return 0;
}
