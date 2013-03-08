#include <linqcpp/Utils/ProgramUtils.h>
#include <linqcpp/linq/Enumerable.h>
#include <iostream>

#include <random>
#include <vector>

using namespace std;

void run(int argc, char* argv[])
{
	//{
	//	PairingHeap<char> heap;
	//	Enumerable<char>::Range('A', 26)
	//		.Concat(Enumerable<char>::Range('a', 26))
	//		.Concat(Enumerable<char>::Range('0', 10))
	//		.Foreach([&](char c){ heap.Insert(c); });
	//	heap.Nodes()
	//		.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//		.Foreach([](char c){ cout << c; });
	//	cout << endl;

	//	auto eee = heap.Nodes(true);
	//	eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//		.Foreach([](char c){ cout << c; });
	//	cout << endl;

	//	//eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//	//	.Foreach([](char c){ cout << c; });
	//	//cout << endl;

	//	//eee.Select<char>([](shared_ptr<PairingHeap<char>::Node> node){ return node->value; })
	//	//	.Foreach([](char c){ cout << c; });
	//	//cout << endl;

	//	cout << "Test case 1:" << endl;

	//	auto enumerable = Enumerable<int>::Range(1, 100)
	//		.SelectMany<pair<int, int>>([](int x){
	//			return Enumerable<int>::From(6)
	//			.ToInclusive(x)
	//			.Select<pair<int, int>>([=](int y){
	//				return std::make_pair(x, y);
	//			});
	//		});

	//	auto myVec = enumerable.OrderByKey<int>([](pair<int, int> p){ return p.second; }).ToVector();

	//	Enumerable<pair<int, int>>::FromRange(myVec)
	//		.ForeachIndexed([](pair<int, int> p, int i){
	//			cout << i << ": " << p.first << ", " << p.second << endl;
	//		});
	//}

	{
		cout << "Test case 2:" << endl;

		auto testing = Enumerable<int>::Generate(12, [](int x){ return x >= -3; }, [](int x){ return x - 1; })
			.ToVector();

		testing = testing;

		auto groups = Enumerable<int>::Range(1, 10)
			.Select<pair<int, Enumerable<int>>>([](int x){
				return make_pair(
					x,
					Enumerable<int>::From(6).ToInclusive(x));
			});

		auto abc = groups.ToVector();

		abc = abc;

		groups
			.Where([](pair<int, Enumerable<int>> p){
				return p.second.Any([](int x){ return x % 2 != 0; });
			})
			.Foreach([](pair<int, Enumerable<int>> group){
				cout << group.first << ":" << endl;
				group.second.Foreach([](int item){
					cout << "\t" << item << endl;
				});
			});

		auto numbers = groups
			.SelectMany<int>([](pair<int, Enumerable<int>> group){
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
