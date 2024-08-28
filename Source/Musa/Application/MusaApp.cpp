// Copyright 2020, Nathan Blane

#include "MusaApp.hpp"
#include "Musa.hpp"
#include "MusaAppWindows.hpp"
#include "File/DirectoryLocations.hpp"
#include "Engine/FrameData.hpp"
#include "Engine/Internal/FrameDataInternal.hpp"
#include "Input/Internal/InputInternal.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Logging/LogFunctions.hpp"
#include "Logging/Sinks/ConsoleWindowSink.hpp"
#include "Logging/Sinks/DebugOutputWindowSink.hpp"
#include "Logging/Sinks/LogFileSink.hpp"

DEFINE_LOG_CHANNEL(AppLog);

MusaApp* gApp = nullptr;
bool gIsRunning = false;

constexpr i32 windowWidth = 1080;
constexpr i32 windowHeight = 720;

MusaApp::MusaApp()
	: inputMap(*this)
{
	uiContext = MakeUnique<UI::Context>(windowWidth, windowHeight);
}

void MusaApp::LaunchApplication()
{
	// TODO - Should be some sort of function...?
	frameTick.Start();
	gIsRunning = true;

	AppInit();

	while (gIsRunning)
	{
		StartFrame();

		// TODO - Should pass this along to the game itself
		ProcessApplicationInputs();
		ProcessApplicationEvents();

		const f32 tick = Frame::GetTickTimeSeconds();
		AppLoop(tick);

		EndFrame();
	}

	AppDeinit();
}

void MusaApp::ResizeWindow(const IntVector2& newDimensions)
{
	appWindow->Resize(newDimensions.x, newDimensions.y);
}

void MusaApp::CloseWindow()
{
	//gameEngine->StopRunningEngine();
	gIsRunning = false;
	appWindow.Reset();
}

void MusaApp::Activation(bool /*activated*/)
{
}

void MusaApp::SetRawMouseInput(bool shouldEnable)
{
	osApp->SetRawMouseInput(shouldEnable, *appWindow);
}

void MusaApp::LockCursor()
{
	IntVector2 position = appWindow->GetPosition();
	Recti rect = {};
	rect.x = position.x;
	rect.y = position.y;
	rect.width = appWindow->GetWidth();
	rect.height = appWindow->GetHeight();
	osApp->LockCursorToRect(rect);
}

void MusaApp::UnlockCursor()
{
	osApp->UnlockCursorFromRect();
}

void MusaApp::ShowCursor(bool showCursor)
{
	osApp->ShowCursor(showCursor);
}

void MusaApp::SetMousePosition(const IntVector2& mousePos)
{
	osApp->SetMousePosition(mousePos);
}

IntVector2 MusaApp::GetMousePosition() const
{
	return osApp->TransformPositionToWindow(*appWindow, osApp->GetMousePosition());
}

void MusaApp::SetApplicationEventCallback(ApplicationEventCallback&& callback)
{
	appEventCallback = MOVE(callback);
}

void MusaApp::SetInputEventCallback(InputEventCallback&& callback)
{
	inputEventCallback = MOVE(callback);
}

void MusaApp::InitializeOSInput()
{
	inputRouter = MakeUnique<ApplicationEventRouter>();
	Input::InitializeInput(inputMap);

	osApp = MakeUnique<MusaAppWindows>();
}

void MusaApp::InitializeApplicationWindow()
{
	MUSA_INFO(AppLog, "Initializing App Window");
	Assert(osApp);
	appWindow = osApp->CreateGameWindow(0, 0, windowWidth, windowHeight);
	Assert(appWindow.IsValid());

	MUSA_DEBUG(AppLog, "Hooking up Input to Window");
	uiContext->SetWindow(*appWindow);
}

void MusaApp::ProcessApplicationInputs()
{
	osApp->ProcessInputEvents(inputMap);
	osApp->ProcessNativeGamepad(inputMap, *inputRouter);

	const auto inputEvents = inputRouter->MoveFrameInputs();
	for (const auto& inputEvent : inputEvents)
	{
		Assert(inputEventCallback);
		inputEventCallback(inputEvent);
	}
}

void MusaApp::ProcessApplicationEvents()
{
	using namespace Musa;

	applicationEvents = inputRouter->MoveApplicationEvents();

	for (const auto& event : applicationEvents)
	{
		switch (event.type)
		{
			case ApplicationEventType::Resize:
			{
				ResizeWindow(IntVector2{ event.resizeData.width, event.resizeData.height });
			}break;

			case ApplicationEventType::Close:
			{
				CloseWindow();
			}break;

			case ApplicationEventType::Activated:
			{
				Activation(true);
			}break;

			case ApplicationEventType::Deactivated:
			{
				Activation(false);
			}break;

			case ApplicationEventType::Invalid:
			default:
			{
				Assert(false);
			}
		}

		// Push event to application
		Assert(appEventCallback);
		appEventCallback(event);
	}
}

void MusaApp::StartFrame()
{
	frameTick.Lap();
	const f32 tick = (f32)frameTick.GetSeconds();
	frameTick.Start();

	Frame::SetFrameStats({ tick });
}

void MusaApp::EndFrame()
{
	applicationEvents.Clear();
}
