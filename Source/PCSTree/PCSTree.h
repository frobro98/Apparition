// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/Uncopyable.hpp"

// TODO - Fix PCSTree implementation because animations don't really compile now...

template<class TreeType>
class PCSTree : private Uncopyable
{
public:
	struct Info
	{
		uint32 currNumNodes = 0;
		uint32 maxNumNodes = 0;
		uint32 currNumLevels = 0;
		uint32 maxNumLevels = 0;
	};

public:
	PCSTree() = default;
	PCSTree(const PCSTree& in) = delete;
	PCSTree& operator=(const PCSTree& in) = delete;

	PCSTree(PCSTree&& other) noexcept;
	PCSTree& operator=(PCSTree&& other) noexcept;

	TreeType* GetRoot() const;

	void Insert(TreeType* const pInNode, TreeType* const pParent);
	void Remove(TreeType* const pInNode);

	void GetInfo(Info& info);
	void Print() const;

public:
	template<bool isConst>
	class Iter
	{
	public:
		using IteratorType = typename std::conditional_t<isConst, const TreeType, TreeType>;

	public:
		Iter(IteratorType* rootNode)
			: root(rootNode), current(rootNode)
		{
		}

		IteratorType *First()
		{
			return root;
		}

		bool IsDone() const
		{
			return current == nullptr;
		}

		IteratorType *CurrentItem() const
		{
			return current;
		}

		Iter& operator++()
		{
			if (current != nullptr)
			{
				current = current->forward;
			}

			return *this;
		}

		IteratorType* operator*()
		{
			return current;
		}

		friend bool operator!=(const Iter& lhs, const Iter& rhs)
		{
			return lhs.current != rhs.current;
		}

	protected:
		IteratorType* root;
		IteratorType* current;
	};

	template<bool isConst>
	class FwdIter : public Iter<isConst>
	{
	public:
		//using IteratorType = typename std::conditional_t<isConst, const TreeType, TreeType>;
	public:
		FwdIter(TreeType *rootNode)
			: Iter<isConst>(rootNode)
		{
		}
	};

// 	template<bool isConst>
// 	class RevIter : public Iter<isConst>
// 	{
// 	public:
// 		using IteratorType = typename std::conditional_t<isConst, const TreeType, TreeType>;
// 	public:
// 		RevIter(TreeType* rootNode)
// 			: Iter<isConst>(
// 				rootNode == nullptr || rootNode->reverse == nullptr ?
// 				rootNode : rootNode->reverse
// 			)
// 		{
// 		}
// 
// 		virtual typename Iter<isConst>::IteratorType* Next() override
// 		{
// 			if (current != nullptr)
// 			{
// 				if (current->reverse == root)
// 				{
// 					current = nullptr;
// 				}
// 				else
// 				{
// 					current = current->reverse;
// 				}
// 			}
// 
// 			return current;
// 		}
// 	};

	using ForwardIterator = FwdIter<false>;
	//using ReverseIterator = RevIter<false>;
	using ConstForwardIterator = FwdIter<true>;
	//using ConstReverseIterator = RevIter <true>;

	friend ForwardIterator begin(PCSTree<TreeType>& tree) { return ForwardIterator(tree.GetRoot()); }
	friend ConstForwardIterator begin(const PCSTree<TreeType>& tree) { return ConstForwardIterator(tree.GetRoot()); }
	friend ForwardIterator end(PCSTree<TreeType>& /*tree*/) { return ForwardIterator(nullptr); }
	friend ConstForwardIterator end(const PCSTree<TreeType>& /*tree*/) { return ConstForwardIterator(nullptr); }

private:
	void PrintTree(TreeType* root) const;
	uint32 TotalLevels(TreeType* root);

	// Data
	Info	treeInfo;
	// TODO - Change this tree representation to some sort of array structure
	TreeType* root = nullptr;
};

template <class TreeType>
PCSTree<TreeType>::PCSTree(PCSTree&& other) noexcept
	: root(other.root)
{
	other.root = nullptr;
}

template <class TreeType>
PCSTree<TreeType>& PCSTree<TreeType>::operator=(PCSTree&& other) noexcept
{
	if (this != &other)
	{
		root = other.root;
		treeInfo = other.treeInfo;

		other.root = nullptr;
	}
	return *this;
}

template <class TreeType>
TreeType* PCSTree<TreeType>::GetRoot() const
{
	return root;
}

template <class TreeType>
void PCSTree<TreeType>::Insert(TreeType* const pInNode, TreeType* const pParent)
{
	Assert(pInNode != nullptr);
	Assert(pInNode != pParent);

	// Adding a child
	if (pParent != nullptr)
	{
		// Set the current child as next sibling
		TreeType* currentChild = pParent->child;
		if (currentChild != nullptr)
		{
			// Parent already has a child
			pInNode->nextSibling = currentChild;
			currentChild->prevSibling = pInNode;
			pInNode->forward = currentChild;
			currentChild->reverse = pInNode;
		}
		// Only here to update the forward and reverse lists
		else
		{
			// Need to find the highest sibling
			TreeType* newForward = pParent;
			while (newForward != nullptr)
			{
				// Node with sibling found
				if (newForward->nextSibling != nullptr)
				{
					newForward = newForward->nextSibling;
					break;
				}
				// Travel up parent list to find next sibling
				newForward = newForward->parent;
			}

			// Set highest sibling to be the next forward
			// Null if farthest right node in tree
			pInNode->forward = newForward;
			if (newForward != nullptr)
			{
				newForward->reverse = pInNode;
			}
			// End of forward list is the beginning of the reverse
			else
			{
				root->reverse = pInNode;
			}
		}

		// Set up Parent/Child relationship
		pInNode->reverse = pParent;
		pParent->forward = pInNode;
		pParent->child = pInNode;
		pInNode->parent = pParent;
	}
	// Adding the root
	else
	{
		root = pInNode;
	}

	++treeInfo.currNumNodes;

	// Don't know if I like this....
	treeInfo.maxNumNodes < treeInfo.currNumNodes ? ++treeInfo.maxNumNodes : 0;
}

template <class TreeType>
void PCSTree<TreeType>::Remove(TreeType* pInNode)
{
	// pInNode would be base case if it got that far
	if (pInNode != nullptr)
	{
		// Remove children
		TreeType* child = pInNode->child;
		while (child != nullptr)
		{
			Remove(child);
			child = pInNode->child;
		}

		// Move parent's child pointer to next sibling
		TreeType* parent = pInNode->parent;
		if (parent != nullptr && parent->child == pInNode)
		{
			pInNode->parent->child = pInNode->nextSibling;
		}
		// Connect the rest of the linked list together
		if (pInNode->nextSibling != nullptr)
		{
			// Connect the next node to the prev node
			pInNode->nextSibling->prevSibling = pInNode->prevSibling;
		}
		if (pInNode->prevSibling != nullptr)
		{
			// Connect the prev node to the next node
			pInNode->prevSibling->nextSibling = pInNode->nextSibling;
		}

		// Update Iterator list
		if (pInNode->reverse != nullptr)
		{
			pInNode->reverse->forward = pInNode->forward;
		}
		if (pInNode->forward != nullptr)
		{
			pInNode->forward->reverse = pInNode->reverse;
		}

		// if we are at the far right of the tree (end of forward list)
		// update root reverse reference
		if (pInNode->forward == nullptr &&
			pInNode->reverse != nullptr)
		{
			root->reverse = pInNode->reverse;
		}

		pInNode->parent = nullptr;
		pInNode->nextSibling = nullptr;
		pInNode->prevSibling = nullptr;
		pInNode->child = nullptr;
		pInNode->forward = nullptr;
		pInNode->reverse = nullptr;

		// Get rid of the rood (special case)
		if (pInNode == root)
		{
			root = nullptr;
		}

		--treeInfo.currNumNodes;
	}
}

template<class TreeType>
void PCSTree<TreeType>::GetInfo(Info &info)
{
	treeInfo.currNumLevels = TotalLevels(root);
	if (treeInfo.currNumLevels > treeInfo.maxNumLevels)
	{
		treeInfo.maxNumLevels = treeInfo.currNumLevels;
	}

	info = treeInfo;
}

template<class TreeType>
void PCSTree<TreeType>::Print() const
{
	PrintTree(root);
}

template<class TreeType>
void PCSTree<TreeType>::PrintTree(TreeType* root) const
{
	if (root != nullptr)
	{
		root->PrintNode();
		TreeType* child = root->GetChild();
		while (child != nullptr)
		{
			PrintTree(child);
			child = child->GetNextSibling();
		}
	}
}

template<class TreeType>
uint32 PCSTree<TreeType>::TotalLevels(TreeType* root)
{
	if (root == nullptr)
	{
		return 0;
	}
	else
	{
		uint32 totalLevels = 1;
		TreeType* child = root->child;
		while (child != nullptr)
		{
			uint32 subTreeLevels = TotalLevels(child) + 1;
			totalLevels = totalLevels < subTreeLevels ? subTreeLevels : totalLevels;
			child = child->nextSibling;
		}
		return totalLevels;
	}
}
