#include <linqcpp/linq/Enumerable.h>
#include "ProgramUtils.h"

#include <string>
#include <random>
#include <vector>
#include <tuple>
#include <functional>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;
using namespace linq;

void TimeIt(string text, int repeatCount, std::function<void ()> f)
{
	auto start = clock();
	for (int i = 0; i < repeatCount; i++)
		f();
	auto stop = clock();
	auto ms = 1000.0 * static_cast<double>(stop - start) / static_cast<double>(CLOCKS_PER_SEC * repeatCount);
	cout << text << " " << ms << endl;
}

TEnumerable<string> FileLines(string path)
{
	return Enumerable::Factory([=]()
	{
		auto stream = make_shared<ifstream>(path);

		return Enumerable::Generate([=]()
			{
				string line;
				std::getline(*stream, line);
				return line;
			})
			.TakeWhile([=](string line){ return !stream->eof(); });
	});
}

void run(int argc, char* argv[])
{
	std::ostream& stream = std::cout;

	std::map<std::string, std::function<bool()>> tests;
	auto AddTest = [&](std::string name, std::function<bool()> func)
	{
		std::map<std::string, std::function<bool()>>::iterator it;
		bool success;
		std::tie(it, success) = tests.insert(std::make_pair(name, func));
		if (!success)
			it->second = []()->bool{ throw std::logic_error("More than one test defined with this same name!"); };
	};
	
	AddTest("FromRange by reference", []()->bool
	{
		std::vector<int> v;
		v.push_back(5);
		return Enumerable::SequenceEqual(
			Enumerable::FromRange(v),
			Enumerable::Return(5));
	});

	AddTest("FromRange by shared_ptr", []()->bool
	{
		auto v = std::make_shared<std::vector<int>>();
		v->push_back(5);
		auto e = Enumerable::FromRange(v);
		v.reset();
		return Enumerable::SequenceEqual(
			e,
			Enumerable::Return(5));
	});

	AddTest("Repeat", []()->bool
	{
		std::vector<int> v;
		for (int i = 0; i < 10; i++)
			v.push_back(5);
		return Enumerable::SequenceEqual(
			Enumerable::FromRange(v),
			Enumerable::Repeat(5).Take(10));
	});

	AddTest("Empty", []()->bool
	{
		std::vector<int> v;
		return Enumerable::SequenceEqual(
			Enumerable::FromRange(v),
			Enumerable::Empty<int>());
	});

	AddTest("Return", []()->bool
	{
		std::vector<int> v;
		v.push_back(5);
		return Enumerable::SequenceEqual(
			Enumerable::FromRange(v),
			Enumerable::Return(5));
	});

	AddTest("Concat", []()->bool
	{
		std::vector<int> v;
		v.push_back(1);
		v.push_back(3);
		v.push_back(4);
		return Enumerable::SequenceEqual(
			Enumerable::FromRange(v),
			Enumerable::Concat(Enumerable::Return(1), Enumerable::Range(3, 2)));
	});

	AddTest("Zip", []()->bool
	{
		std::vector<int> v1;
		v1.push_back(-4);
		v1.push_back(2);
		v1.push_back(1);
		std::vector<int> v2;
		v2.push_back(4);
		v2.push_back(-1);
		v2.push_back(1);
		return Enumerable::SequenceEqual(
			Enumerable::Range(0, 3),
			Enumerable::Zip(Enumerable::FromRange(v1), Enumerable::FromRange(v2), [](int a, int b){ return a + b; }));
	});

	int successCount = 0;
	int failureCount = 0;
	std::vector<std::string> failureNames;
	for (auto test = tests.begin(); test != tests.end(); ++test)
	{
		const auto& name = test->first;
		const auto& func = test->second;

		stream << "========================================" << std::endl;
		stream << "Test: " << name << std::endl;
		bool success = false;
		try
		{
			success = func();
		}
		catch(std::exception& e)
		{
			stream << "Exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			stream << "Exception: <unknown>" << std::endl;
		}
		stream << (success ? "Success" : "Failure") << std::endl;
		(success ? successCount : failureCount)++;
		if (!success)
			failureNames.push_back(name);
	}

	stream << "========================================" << std::endl;
	stream << std::endl;
	stream << "Success count: " << successCount << std::endl;
	stream << "Failure count: " << failureCount << std::endl;
	stream << "Failed tests:" << std::endl;
	for (auto failureName = failureNames.begin(); failureName != failureNames.end(); ++failureName)
		stream << "    " << *failureName << std::endl;

	//Wait for user input
	string junk;
	getline(cin, junk);
}

int main(int argc, char* argv[])
{
	return ProgramUtils::run(argc, argv, run);
}
