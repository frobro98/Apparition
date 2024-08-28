// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "Debugging/DebugOutput.hpp"

template <class TreeType>
class PCSNode
{
public:
	PCSNode() = default;
	virtual ~PCSNode() = default;

	PCSNode(TreeType* pInParent, TreeType* pInChild, TreeType* pInNextSibling, TreeType* pInPrevSibling, const tchar* pInName);
	PCSNode(const tchar* pInName);

	// print
	void PrintNode() const;
	void PrintChildren() const;
	void PrintSiblings() const;

	// get number of children/siblings
	int GetNumSiblings() const;
	int GetNumChildren() const;

public:
	String name;

	TreeType* parent = nullptr;
	TreeType* child = nullptr;
	TreeType* nextSibling = nullptr;
	TreeType* prevSibling = nullptr;

	TreeType* forward = nullptr;
	TreeType* reverse = nullptr;
};

template<class TreeType>
PCSNode<TreeType>::PCSNode(TreeType* pInParent, TreeType* pInChild, TreeType* pInNextSibling, TreeType* pInPrevSibling, const tchar* inName)
	: name(inName),
	parent(pInParent),
	child(pInChild),
	nextSibling(pInNextSibling),
	prevSibling(pInPrevSibling)
{
}

template<class TreeType>
PCSNode<TreeType>::PCSNode(const tchar* inName)
	: name(inName)
{
}

template<class TreeType>
void PCSNode<TreeType>::PrintNode() const
{
	Debug::Print("---------------------------\n");
	Debug::Printf("Node: %s\n", *name);
	Debug::Printf("Address: %p\n", this);
	Debug::Print("---------------------------\n\n");
}

template<class TreeType>
void PCSNode<TreeType>::PrintChildren() const
{
	TreeType* child = child;
	while (child != nullptr)
	{
		child->PrintNode();
		child = child->pNextSibling;
	}
}

template<class TreeType>
void PCSNode<TreeType>::PrintSiblings() const
{
	TreeType* sibling;
	if (parent->pChild != this)
	{
		sibling = parent->pChild;
	}
	else
	{
		sibling = nextSibling;
	}
	while (sibling != nullptr)
	{
		sibling->PrintNode();
		sibling = sibling->pNextSibling;
	}
}

template<class TreeType>
int PCSNode<TreeType>::GetNumSiblings() const
{
	int numSibs = 0;
	TreeType* currentSib;
	// Switch to child of parent to walk the entire list
	if (parent != nullptr && parent->pChild != this)
	{
		currentSib = parent->GetChild();
	}
	// Count myself and walk the rest of the list
	else
	{
		++numSibs;
		currentSib = nextSibling;
	}
	while (currentSib != nullptr)
	{
		++numSibs;
		currentSib = currentSib->pNextSibling;
	}

	return numSibs;
}

template<class TreeType>
int PCSNode<TreeType>::GetNumChildren() const
{
	int numChildren = 0;
	TreeType* currentChild = child;
	while (currentChild != nullptr)
	{
		++numChildren;
		currentChild = currentChild->pNextSibling;
	}

	return numChildren;
}