// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "BasicTypes/Function.hpp"
#include "Application/MusaAppOS.hpp"
#include "Window/Window.h"
#include "UI/UIContext.hpp"
#include "Time/EngineTick.h"
#include "Input/ApplicationInputMap.hpp"
#include "Input/ApplicationEventRouter.hpp"
#include "MusaAPI.hpp"

class MUSA_API MusaApp
{
public:
	using ApplicationEventCallback = Function<void(const Musa::ApplicationEvent&)>;
	using InputEventCallback = Function<void(const Input::Event&)>;

public:
	MusaApp();
	virtual ~MusaApp() {}

	void LaunchApplication();

	virtual void AppInit() = 0;
	virtual void AppLoop(f32 tick) = 0;
	virtual void AppDeinit() = 0;

	forceinline ApplicationInputMap& GetInputMap() { return inputMap; }
	forceinline ApplicationEventRouter& GetInputRouter() { return *inputRouter; }

	// Application events
	void ResizeWindow(const IntVector2& newDimensions);
	void CloseWindow();
	void Activation(bool activated);

	void SetRawMouseInput(bool shouldEnable);
	void LockCursor();
	void UnlockCursor();
	void ShowCursor(bool showCursor);
	void SetMousePosition(const IntVector2& mousePos);
	// Gets the position of the cursor in the application window itself
	IntVector2 GetMousePosition() const;

	void SetApplicationEventCallback(ApplicationEventCallback&& callback);
	void SetInputEventCallback(InputEventCallback&& callback);

protected:
	void InitializeOSInput();
	void InitializeApplicationWindow();

protected:
	ApplicationInputMap inputMap;
	UniquePtr<UI::Context> uiContext;
	UniquePtr<Window> appWindow;
	UniquePtr<ApplicationEventRouter> inputRouter;
	UniquePtr<MusaAppOS> osApp;

private:
	void StartFrame();
	void EndFrame();
	void ProcessApplicationInputs();
	void ProcessApplicationEvents();

private:
	EngineTick frameTick;
	DynamicArray<Musa::ApplicationEvent> applicationEvents;
	ApplicationEventCallback appEventCallback;
	InputEventCallback inputEventCallback;
};
