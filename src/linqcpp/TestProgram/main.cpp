#include <linqcpp/Utils/ProgramUtils.h>
#include <linqcpp/linq/Enumerable.h>

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
	return Enumerable::Factory<string>([=]()
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

	lines.OrderBy([](string line){ return -static_cast<int>(line.length()); })
		.Take(10)
		.ForEach([](string line){ cout << line << endl; });

	//auto v = Enumerable::Sequence(0).Take(100).ToVector();
	////cout << Enumerable::FromRange<int>(v).ToString() << endl;

	//cout << "Select, ToVector" << endl;
	//TimeIt("linq:", 100, [=](){
	//	vector<int> v2(10);
	//	Enumerable::FromRange<int>(v)
	//		.Select<int>([](int x){ return x % 5; })
	//		.Where([](int x){ return x % 2 != 0; })
	//		.Order()
	//		.Take(10)
	//		.IntoVector(v2);
	//});

	//TimeIt("std:", 100, [=](){
	//	vector<int> v2(v.size());
	//	for (auto it = v.begin(); it != v.end(); ++it)
	//		if (*it % 2 != 0)
	//			v2.push_back((*it) % 5);
	//	sort(v2.begin(), v2.end());
	//	vector<int> v3(v2.begin(), v2.begin() + 10);
	//});

	string junk;
	getline(cin, junk);
}

void abc(){


	//{
	//	PairingHeap<char> heap;
	//	TEnumerable<char>::Range('A', 26)
	//		.Concat(TEnumerable<char>::Range('a', 26))
	//		.Concat(TEnumerable<char>::Range('0', 10))
	//		.ForEach([&](char c){ heap.Insert(c); });
	//	heap.Nodes()
	//		.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//		.ForEach([](char c){ cout << c; });
	//	cout << endl;

	//	auto eee = heap.Nodes(true);
	//	eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//		.ForEach([](char c){ cout << c; });
	//	cout << endl;

	//	//eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//	//	.ForEach([](char c){ cout << c; });
	//	//cout << endl;

	//	//eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//	//	.ForEach([](char c){ cout << c; });
	//	//cout << endl;

	//	cout << "Test case 1:" << endl;

	//	auto enumerable = TEnumerable<int>::Range(1, 100)
	//		.SelectMany<pair<int, int>>([](int x){
	//			return TEnumerable<int>::From(6)
	//			.ToInclusive(x)
	//			.Select<pair<int, int>>([=](int y){
	//				return std::make_pair(x, y);
	//			});
	//		});

	//	auto myVec = enumerable.OrderByKey<int>([](pair<int, int> p){ return p.second; }).ToVector();

	//	TEnumerable<pair<int, int>>::FromRange(myVec)
	//		.ForEachIndexed([](pair<int, int> p, int i){
	//			cout << i << ": " << p.first << ", " << p.second << endl;
	//		});
	//}

	{
		cout << "Test case 2:" << endl;

		auto testing = Enumerable::Sequence(12, [](int x){ return x >= -3; }, [](int x){ return x - 1; })
			.SelectMany<double>([](int x)
			{
				return Enumerable::Sequence(0.0, [](double y){ return y + 0.1; })
					.Take(11)
					.Select([=](double y) { return y + x; });
			})
			.Index()
			.ToVector();

		testing = testing;

		auto testing2 = Enumerable::Sequence(1, [](int x){ return (x + 7) % 5; }).Take(33).StaticCast<double>().ToVector();

		testing2 = testing2;

		auto groups = Enumerable::Range(1, 10)
			.Select([](int x){
				return make_pair(
					x,
					Enumerable::Sequence(6).ToInclusive(x));
			});

		auto abc = groups.ToVector();

		abc = abc;

		groups
			.Where([](pair<int, TEnumerable<int>> p){
				return p.second.Any([](int x){ return x % 2 != 0; });
			})
			.ForEach([](pair<int, TEnumerable<int>> group){
				cout << group.first << ":" << endl;
				group.second.ForEach([](int item){
					cout << "\t" << item << endl;
				});
			});

		auto numbers = groups
			.SelectMany<int>([](pair<int, TEnumerable<int>> group){
				return group.second;
			})
			.StaticCast<double>()
			.Select([](double x){ return x + 0.5; });

		//cout << numbers.ToString(",") << endl;
		cout << "Min: " << numbers.Min() << endl;
		cout << "Max: " << numbers.Max() << endl;
		cout << "Avg: " << numbers.Average() << endl;

		string junk;
		getline(cin, junk);
	}
}

int main(int argc, char* argv[])
{
	return ProgramUtils::run(argc, argv, run);
}
