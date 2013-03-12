#pragma once

#include "dllapi.h"

#include <memory>
#include <functional>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "Comparer.h"
#include "Enumerable.h"

namespace linq {

template<typename T>
class TEnumerable;

template<typename T>
class PairingHeap
{
public:
	class Node
	{
		friend PairingHeap;

	public:
		T value;

		bool IsActive()
		{
			return isActive;
		}

	private:
		bool isActive;
		std::weak_ptr<Node> left;
		std::shared_ptr<Node> right;
		std::shared_ptr<Node> firstChild;
		Node(T value)
			: value(value)
			, isActive(false)
			, left(std::weak_ptr<Node>())
			, right(nullptr)
			, firstChild(nullptr)
		{
		}
	};

	PairingHeap()
		: comparer(Comparer::Default<T>())
		, root(nullptr)
		, count(0)
	{
	}

	PairingHeap(std::function<int (T, T)> comparer)
		: comparer(comparer)
		, root(nullptr)
		, count(0)
	{
	}

	bool Empty()
	{
		return !root;
	}

	int Count()
	{
		return count;
	}

	std::shared_ptr<Node> Insert(T value)
	{
		std::shared_ptr<Node> node(new Node(value));
		Insert(node);
		return node;
	}

	void Insert(std::shared_ptr<Node> node)
	{
		if (!node)
		{
			return;
		}
		if (node->isActive)
		{
			throw std::runtime_error("Cannot insert active node.");
		}
		++count;
		root = Pair(root, node);
		node->isActive = true;
	}

	void Remove(std::shared_ptr<Node> node)
	{
		if (!node->isActive)
		{
			throw std::runtime_error("Cannot remove inactive node.");
		}
		--count;
		if (node == root)
		{
			root = RemoveRoot(root);
		}
		else
		{
			SpliceOut(node);
			root = Pair(root, RemoveRoot(node));
		}
		node->isActive = false;
	}

	std::shared_ptr<Node> GetMin()
	{
		return root;
	}

	std::shared_ptr<Node> ExtractMin()
	{
		auto node = root;
		Remove(node);
		return node;
	}

	TEnumerable<std::shared_ptr<Node>> Nodes(bool ordered = false)
	{
		if (!ordered)
		{
			return Nodes(root);
		}

		struct State
		{
			State() { }
			std::shared_ptr<PairingHeap<T>> clone;
			std::shared_ptr<Node> value;
		};

		auto _this = std::make_shared<PairingHeap<T>>(*this);

		return TEnumerable<std::shared_ptr<Node>>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->clone = std::make_shared<PairingHeap<T>>(_this->Clone());

				return std::make_shared<TEnumerator<std::shared_ptr<Node>>>
				(
					[=]()
					{
						if (!state->clone->IsEmpty())
						{
							state->value = state->clone->ExtractMin();
							return true;
						}
						else
						{
							return false;
						}
					},
					[=]()
					{
						return state->value;
					}
				);
			}
		);
	}

	PairingHeap Clone()
	{
		PairingHeap clone;
		clone.comparer = comparer;
		clone.root = Clone(root);
		clone.count = count;
		return clone;
	}

private:
	std::function<int (T, T)> comparer;
	std::shared_ptr<Node> root;
	int count;

	std::shared_ptr<Node> Pair(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2)
	{
		if (!n1)
		{
			return n2;
		}
		if (!n2)
		{
			return n1;
		}
		if (!n1->left.expired() || !n2->left.expired())
		{
			throw std::runtime_error("Can only pair two root nodes.");
		}
		if (comparer(n1->value, n2->value) < 0)
		{
			std::swap(n1, n2);
		}
		auto c = n2->firstChild;
		n1->left = n2;
		n1->right = c;
		if (c)
		{
			c->left = n1;
		}
		n2->firstChild = n1;
		return n2;
	}

	static void SpliceOut(std::shared_ptr<Node> n)
	{
		auto n_left = n->left.lock();
		if (!n_left)
		{
			return;
		}
		if (n_left->firstChild == n)
		{
			auto p = n_left;
			if (!n->right)
			{
				p->firstChild.reset();
			}
			else
			{
				auto r = n->right;
				r->left = p;
				p->firstChild = r;
			}
		}
		else
		{
			auto l = n_left;
			auto r = n->right;
			l->right = r;
			if (r)
			{
				r->left = l;
			}
		}
		n->left.reset();
		n->right.reset();
	}

	std::shared_ptr<Node> RemoveRoot(std::shared_ptr<Node> n)
	{
		if (!n->left.expired())
		{
			throw std::runtime_error("RemoveRoot cannot be applied to non-root nodes.");
		}
		auto c1 = n->firstChild;
		if (!c1)
		{
			return nullptr;
		}
		if (c1->right)
		{
			auto c2 = c1->right;
			SpliceOut(c1);
			SpliceOut(c2);
			return Pair(Pair(c1, c2), RemoveRoot(n));
		}
		else
		{
			c1->left.reset();
			n->firstChild.reset();
			return c1;
		}
	}

	//Enumerable over the nodes in the given subtree
	static TEnumerable<std::shared_ptr<Node>> Nodes(std::shared_ptr<Node> subtreeRoot)
	{
		return TEnumerable<std::shared_ptr<Node>>::Concat
		(
			TEnumerable<std::shared_ptr<Node>>::Return(subtreeRoot),

			TEnumerable<std::shared_ptr<Node>>::Sequence
			(
				subtreeRoot->firstChild,
				[](std::shared_ptr<Node> child){ return child != nullptr; },
				[](std::shared_ptr<Node> child){ return child->right; }
			)
			.SelectMany<std::shared_ptr<Node>>
			(
				[](std::shared_ptr<Node> child){ return PairingHeap<T>::Nodes(child); }
			)
		);
	}

	static std::shared_ptr<Node> Clone(std::shared_ptr<Node> node)
	{
		if (!node)
		{
			return nullptr;
		}
		std::shared_ptr<Node> clone(new Node(node->value));
		clone->isActive = node->isActive;
		clone->right = Clone(node->right);
		clone->right->left = clone;
		clone->firstChild = Clone(node->firstChild);
		clone->firstChild->left = clone;
		return clone;
	}
};

}