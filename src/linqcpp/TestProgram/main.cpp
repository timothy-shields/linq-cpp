#include <linqcpp/Utils/ProgramUtils.h>
#include <linqcpp/linq/Enumerable.h>
#include <iostream>

#include <random>
#include <vector>

using namespace std;
using namespace linq;

void run(int argc, char* argv[])
{
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

		auto testing = Enumerable::Generate<int>(12, [](int x){ return x >= -3; }, [](int x){ return x - 1; })
			.SelectMany<double>([](int x)
			{
				return Enumerable::Generate<double>(0, [](double y){ return y + 0.1; })
					.Take(11)
					.Select<double>([=](double y) { return y + x; });
			})
			.ToVector();

		testing = testing;

		auto groups = Enumerable::Range(1, 10)
			.Select<pair<int, TEnumerable<int>>>([](int x){
				return make_pair(
					x,
					Enumerable::From(6).ToInclusive(x));
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
			.Select<double>([](double x){ return x + 0.5; });

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
