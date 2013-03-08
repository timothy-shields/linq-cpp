#pragma once

#include "dllapi.h"
#include <functional>

class Utils_API ProgramUtils
{
public:
	static int run(int argc, char* argv[], std::function<void (int, char* [])> run);
};
