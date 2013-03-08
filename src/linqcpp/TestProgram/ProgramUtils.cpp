#include "ProgramUtils.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

int ProgramUtils::run(int argc, char* argv[], function<void (int, char* [])> run)
{
	try {
		run(argc, argv);
	} catch(exception& e) {
		cout << "The program crashed from the following exception: " << e.what() << endl;
		return EXIT_FAILURE;
	} catch(...) {
		cout << "The program crashed from an exception of an unknown type." << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
