#include <linqcpp/linq/interactive.h>
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
//using namespace linq;

void TimeIt(string text, int repeatCount, std::function<void ()> f)
{
	auto start = clock();
	for (int i = 0; i < repeatCount; i++)
		f();
	auto stop = clock();
	auto ms = 1000.0 * static_cast<double>(stop - start) / static_cast<double>(CLOCKS_PER_SEC * repeatCount);
	cout << text << " " << ms << endl;
}

//TEnumerable<string> FileLines(string path)
//{
//	return Enumerable::Factory([=]()
//	{
//		auto stream = make_shared<ifstream>(path);
//
//		return Enumerable::Generate([=]()
//			{
//				string line;
//				std::getline(*stream, line);
//				return line;
//			})
//			.TakeWhile([=](string line){ return !stream->eof(); });
//	});
//}

//std::shared_ptr<enumerable<string>> FileLines(string path)
//{
//	return interactive<void>::factory([=]()
//	{
//		auto stream = make_shared<ifstream>(path);
//
//		return interactive<void>::generate([=]()
//			{
//				string line;
//				std::getline(*stream, line);
//				return line;
//			})
//			.TakeWhile([=](string line){ return !stream->eof(); });
//	});
//}

void run(int argc, char* argv[])
{
	auto seq = ix::for_(0, [](int n){ return n < 10; }, [](int n){ return n + 1; })
		.to_vector();
	auto ffff = [](int n)
	{
		return ix::for_(0, [](int i){ return i < 3; }, [](int i){ return i + 1; })
			.ref_count();
			//.select([=](int i){ return n + i; });
	};

	auto r = ix::for_(0, [](int n){ return n < 10000000; }, [](int n){ return n + 1; })
		.to_vector();

	//std::vector<double> output;
	//output.reserve(10000000);

	//ix::from(r.rbegin(), r.rend())
	//	.static_cast_<double>()
	//	.select([](double n){ return std::sqrt(n); })
	//	.into_vector(output);

	auto rrr = ix::iota(0).take(100).to_vector();

	auto vvv = ix::from(seq.begin(), seq.end())
		.select([](int n){ return 4 * n; })
		.select([](int n){ return n - 2; })
		.memoize()
		.where([](int n){ return (n % 2) == 0; })
		.select_many([](int n)
	    {
			return ix::for_(n, [=](int k){ return k < (n+3); }, [](int k){ return k + 1; })
				.capture();
		})
		.skip(3)
		.take(15)
		.skip_until([](int n){ return n > 10; })
		.minmax();

	auto vvv2 = ix::from(seq.begin(), seq.end())
		.select([](int n){ return 3 * n - 2; })
		.to_vector();

	//auto www = ix::capture(vvv)
		//._where([](int n){ return (n % 2) == 0; })
		//.select([](int n){ return -static_cast<double>(n); })
		//.select_many(ffff)
		//.to_vector();

	std::string junk;
	std::getline(std::cin, junk);
	return;
}

//void run2(int argc, char* argv[])
//{
//	//vvv = vvv;
//
//	std::ostream& stream = std::cout;
//
//	std::map<std::string, std::function<bool()>> tests;
//	auto AddTest = [&](std::string name, std::function<bool()> func)
//	{
//		std::map<std::string, std::function<bool()>>::iterator it;
//		bool success;
//		std::tie(it, success) = tests.insert(std::make_pair(name, func));
//		if (!success)
//			it->second = []()->bool{ throw std::logic_error("More than one test defined with this same name!"); };
//	};
//	
//	AddTest("FromRange by reference", []()->bool
//	{
//		std::vector<int> v;
//		v.push_back(5);
//		return Enumerable::SequenceEqual(
//			Enumerable::FromRange(v),
//			Enumerable::Return(5));
//	});
//
//	AddTest("FromRange by shared_ptr", []()->bool
//	{
//		auto v = std::make_shared<std::vector<int>>();
//		v->push_back(5);
//		auto e = Enumerable::FromRange(v);
//		v.reset();
//		return Enumerable::SequenceEqual(
//			e,
//			Enumerable::Return(5));
//	});
//
//	AddTest("Repeat", []()->bool
//	{
//		return Enumerable::SequenceEqual(
//			Enumerable::Return(5, 5, 5),
//			Enumerable::Repeat(5).Take(3));
//	});
//
//	AddTest("Empty", []()->bool
//	{
//		std::vector<int> v;
//		return Enumerable::SequenceEqual(
//			Enumerable::FromRange(v),
//			Enumerable::Empty<int>());
//	});
//
//	AddTest("Return", []()->bool
//	{
//		std::vector<int> v;
//		v.push_back(5);
//		return Enumerable::SequenceEqual(
//			Enumerable::FromRange(v),
//			Enumerable::Return(5));
//	});
//
//	AddTest("Concat", []()->bool
//	{
//		std::vector<int> v;
//		v.push_back(1);
//		v.push_back(3);
//		v.push_back(4);
//		return Enumerable::SequenceEqual(
//			Enumerable::FromRange(v),
//			Enumerable::Concat(Enumerable::Return(1), Enumerable::Range(3, 2)));
//	});
//
//	AddTest("Zip", []()->bool
//	{
//		return Enumerable::SequenceEqual(
//			Enumerable::Range(0, 3),
//			Enumerable::Zip(
//				Enumerable::Return(-4, 2, 1),
//				Enumerable::Return(4, -1, 1),
//				[](int a, int b){ return a + b; }));
//	});
//
//	AddTest("GroupBy", []()->bool
//	{
//		auto e = Enumerable::Return(1, 2, 3, 3, 0, 5)
//			.GroupBy([](int x){ return x % 3; });
//		return e.Count() == 3
//			&& e.ElementAt(0).first == 0 && Enumerable::SequenceEqual(e.ElementAt(0).second, Enumerable::Return(3, 3, 0))
//			&& e.ElementAt(1).first == 1 && Enumerable::SequenceEqual(e.ElementAt(1).second, Enumerable::Return(1))
//			&& e.ElementAt(2).first == 2 && Enumerable::SequenceEqual(e.ElementAt(2).second, Enumerable::Return(2, 5));			
//	});
//
//	int successCount = 0;
//	int failureCount = 0;
//	std::vector<std::string> failureNames;
//	for (auto test = tests.begin(); test != tests.end(); ++test)
//	{
//		const auto& name = test->first;
//		const auto& func = test->second;
//
//		stream << "========================================" << std::endl;
//		stream << "Test: " << name << std::endl;
//		bool success = false;
//		try
//		{
//			success = func();
//		}
//		catch(std::exception& e)
//		{
//			stream << "Exception: " << e.what() << std::endl;
//		}
//		catch(...)
//		{
//			stream << "Exception: <unknown>" << std::endl;
//		}
//		stream << (success ? "Success" : "Failure") << std::endl;
//		(success ? successCount : failureCount)++;
//		if (!success)
//			failureNames.push_back(name);
//	}
//
//	stream << "========================================" << std::endl;
//	stream << std::endl;
//	stream << "Success count: " << successCount << std::endl;
//	stream << "Failure count: " << failureCount << std::endl;
//	stream << "Failed tests:" << std::endl;
//	for (auto failureName = failureNames.begin(); failureName != failureNames.end(); ++failureName)
//		stream << "    " << *failureName << std::endl;
//
//	//Wait for user input
//	string junk;
//	getline(cin, junk);
//}

int main(int argc, char* argv[])
{
	return ProgramUtils::run(argc, argv, run);
}
