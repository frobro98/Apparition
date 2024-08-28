// Copyright 2020, Nathan Blane

#include "WidgetBatchElemements.hpp"

BatchElement& WidgetBatchElements::GetBatchElement(const NativeTexture* batchTexture)
{
	BatchElement* elem = batchElements.FindFirstUsing([batchTexture](BatchElement& e)
		{
			return e.batchTexture == batchTexture;
		});

	if (elem == nullptr)
	{
		u32 i = batchElements.AddDefault();
		elem = &batchElements[i];
		elem->batchTexture = batchTexture;
	}

	return *elem;
}
