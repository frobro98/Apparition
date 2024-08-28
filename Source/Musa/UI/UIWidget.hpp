// Copyright 2020, Nathan Blane

#pragma once

#include <optional>

#include "Containers/DynamicArray.hpp"
#include "Input/IInputReceiver.hpp"
#include "Math/Vector2.hpp"
#include "Math/Matrix2.hpp"

class WidgetBatchElements;

namespace UI
{

// TODO - Figure out a way to represent gpu side data!!

// A widget is just a general term for whatever will exist in UI.
// It could be some sort of interactable widget like a button or it could be something that is only used for layout. 
struct Widget : public IInputReceiver
{
public:
	Widget() = default;
	Widget(Widget* parentUI) : parent(parentUI) {}
	virtual ~Widget() = default;


	// UI must implement the IInputReceiver interface to receive all the different input
	//virtual InputEvents OnControllerAnalogChange(u32 /*controllerIndex*/, Input::Buttons /*analogInput*/, f32 /*analogValue*/) { return InputEvents{}; }
	virtual InputEvents OnMouseMove(const IntVector2& /*currentMousePos*/, const IntVector2& /*prevMousePos*/, const IntVector2& /*delta*/) { return InputEvents{}; }
	virtual InputEvents OnMouseEnter(const IntVector2& /*currentMousePos*/, const IntVector2& /*prevMousePos*/, const IntVector2& /*delta*/) { return InputEvents{}; }
	virtual InputEvents OnMouseExit(const IntVector2& /*currentMousePos*/, const IntVector2& /*prevMousePos*/, const IntVector2& /*delta*/) { return InputEvents{}; }
	virtual InputEvents OnMouseDown(Input::Buttons /*input*/) { return InputEvents{}; }
	virtual InputEvents OnMouseUp(Input::Buttons /*input*/) { return InputEvents{}; }
	virtual InputEvents OnKeyDown(Input::Buttons /*input*/, bool /*isRepeated*/) { return InputEvents{}; }
	virtual InputEvents OnKeyUp(Input::Buttons /*input*/) { return InputEvents{}; }
	virtual InputEvents OnChar(tchar /*c*/, bool /*isRepeated*/) { return InputEvents{}; }
	virtual InputEvents OnFocusReceived() { return InputEvents{}; }
	virtual InputEvents OnFocusLost() { return InputEvents{}; }

	virtual InputEvents OnActivationChanged(bool /*activated*/) { return InputEvents{}; }

	// Widget public interface
	
	// Updates widget hierarchy
	void Update(const Vector2& parentPosition, const Vector2& parentScale);
	// Prepares widget hierarchy for render;
	void PrepareForRender(WidgetBatchElements& widgetElements);

public:
	DynamicArray<Widget*> children;

	Widget* parent = nullptr;

	// UI needs position information relative to the parent
	Vector2 relativePosition = Vector2(0, 0);
	// UI needs some sort of transform for rendering
	Vector2 relativeScale = Vector2(1, 1); // Also represents the dimensions...???

	// UI needs an anchor point and pivot point
	Vector2 pivot;

	// Actual transforms within view space
	Vector2 absolutePosition;
	Vector2 absoluteScale;

private:
	void UpdateAbsoluteTransform(const Vector2& parentPosition, const Vector2& parentScale);

private:
	// This function exists to allow widgets to execute behavior that's specific to them
	// Currently, there isn't any reason to have it be more complicated
	virtual void UpdateInternal() = 0;
	
	/* Ideas for how to handle UI rendering:
			1) Widget locally has render elements (quads, vertex descriptions, etc.).
			   These elements are then given to some sort of rendering batch system upon 
			   render/prepare render? Then once the tree has been fully traversed, the 
			   render system draws the primitives

			   ***This idea might not make much sense because it relies on the widget knowing 
			   about its own rendering and then an extra copy of data (not necessarily a huge deal...)

			2) Upon calling PrepareForRenderInternal, the widget gets passed a batch of UI primitives.
			   In the render preparation, the widget must describe its own geometry using the batch of 
			   primitives and once the tree has been fully traversed, the render system renders them.

			   ***This solution is better than the first solution because the widget doesn't store anything
			   related to its rendering. It constructs it on the fly. 
			   However, it could be argued that this
			   solution breaks the current Musa paradigm of separating the cpu and gpu data. 
			   It could also be viewed that it doesn't do this at all. The gpu data doesn't really 
			   "exist" in this solution, and the cpu data in every system in Musa that uses rendering 
			   gets "pushed" to the gpu. For some other systems, the gpu data just lives in their own 
			   encapsulated structure that receives the cpu data. UI primitives are more simplistic (quads), 
			   but the way they are rendered is much more complex, so having specific gpu data stores that 
			   understand the nuances of how a widget should be rendered is super overkill
	*/
	virtual void PrepareForRenderInternal(WidgetBatchElements& widgetElements) = 0;
};
}
