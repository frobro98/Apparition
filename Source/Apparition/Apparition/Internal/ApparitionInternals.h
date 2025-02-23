#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/Internal/DeviceManager.h"

struct ApparitionInternals
{
	Apparition::AllocationCallbacks allocCallbacks;
	DeviceManager* deviceManager = nullptr;
	Apparition::ValidationDelegate userValidationDelegate;
	void* validationUserData = nullptr;
};

extern ApparitionInternals apparition;
