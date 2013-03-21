#pragma once

#include <functional>

class ProgramUtils
{
public:
	static int run(int argc, char* argv[], std::function<void (int, char* [])> run);
};
