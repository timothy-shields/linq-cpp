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
	auto lines = FileLines("C:\\Users\\Timothy\\Documents\\GitHub\\linq-cpp\\src\\CMakeLists.txt");

	cout << "30 longest lines:" << endl << endl;

	lines.OrderBy(Ordering::Descending, [](string& line){ return line.length(); })
		.Take(30)
		.ForEach([](string& line){ cout << line.size() << ":\t||" << line << "||" << endl; });

	cout << endl << endl;

	cout << "30 shortest lines:" << endl << endl;

	lines.OrderBy(Ordering::Ascending, [](string& line){ return line.length(); })
		.Take(30)
		.ForEach([](string& line){ cout << line.size() << ":\t||" << line << "||" << endl; });

	cout << endl << endl;

	cout << "grouped lines:" << endl << endl;

	lines.GroupBy([](string& line){ return line.length(); })
		.ForEach([](pair<size_t, TEnumerable<string>> group){ cout << group.first << "(" << group.second.Count() << "):" << endl << group.second.ToString("\n") << endl; });

	cout << endl << endl;

	cout << Enumerable::Sequence(0)
		.Take(100)
		.GroupBy([](int x){ return x % 10; })
		.ToString("\n",
			[](std::stringstream& stream, std::pair<int, TEnumerable<int>> _pair)
			{
				stream << "n % 10 == " << _pair.first << ": " << _pair.second.ToString();
			})
		<< endl;

	auto v(Enumerable::Range(0, 1000).Select([](int x){ return (x * 2147483647) % 400; }).ToVector());

	cout << "Select, ToVector" << endl;
	TimeIt("linq:", 10, [=](){
		vector<int> v2(10);
		Enumerable::FromRange(v)
			.Select([](int x){ return x % 5; })
			.Where([](int x){ return x % 2 != 0; })
			.Order(Ordering::Ascending)
			.Take(10)
			.IntoVector(v2);
	});

	TimeIt("std:", 10, [=](){
		vector<int> v2(v.size());
		for (auto it = v.begin(); it != v.end(); ++it)
			if (*it % 2 != 0)
				v2.push_back((*it) % 5);
		sort(v2.begin(), v2.end());
		vector<int> v3(v2.begin(), v2.begin() + 10);
	});

	string junk;
	getline(cin, junk);
}

int main(int argc, char* argv[])
{
	return ProgramUtils::run(argc, argv, run);
}
