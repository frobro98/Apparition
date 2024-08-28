// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

namespace UI
{
struct Widget;

// Represents a path within the UI tree
class Path
{
public:
	Path() = default;
	Path(DynamicArray<Widget*>&& widgetPath);

	u32 Size() const;
	bool ContainsWidget(const Widget& widget) const;

public:
	class Iterator final
	{
	public:
		Iterator(Path& path)
			: widgets(path.path.GetData()),
			currentIndex((i32)path.Size() - 1)
		{
		}

		Iterator(nullptr_t)
			: widgets(nullptr),
			currentIndex(-1)
		{
		}

		Iterator& operator++()
		{
			if (currentIndex >= 0)
			{
				--currentIndex;
			}
			else
			{
				widgets = nullptr;
			}
			return *this;
		}

		bool operator!=(const Iterator& other)
		{
			return other.widgets == widgets &&
				other.currentIndex == currentIndex;
		}

		Widget& operator*() const
		{
			Assert(widgets);
			return *widgets[currentIndex];
		}

	private:
		Widget** widgets;
		i32 currentIndex;
	};

private:
	DynamicArray<Widget*> path;
};
}
