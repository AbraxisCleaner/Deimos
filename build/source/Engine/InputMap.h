#if !defined(_ENGINE_INPUT_MAP_H_)
#define _ENGINE_INPUT_MAP_H_

#include <pch.h>
#include "InputKey.inl"
#include <Stl/String.h>
#include <Stl/Container.h>

#include <functional>

// - Input events are stored in SInputActions.
// - Each SInputAction stores:
//    * The devices that the action will read from.
//    * The primitive that the action will relay to any listeners.
//    * The conditions that must be met for any listeners to be notified.
//    * The SInputBindingGroups of the action.
//    * The CInputListeners that 
// - Each SInputBindingGroup stores:
//    * The device that the group represents (keyboard/mouse/gamepad).
//    * The individual bindings.
// - Each SInputBinding stores:
//    * The device property that is checked (keyboard->key, gamepad->button/stick/trigger, mouse->button/delta)
//
// When an action's conditions are met, all listeners registered to that action have their 'OnAction()' method called.


enum EInputDevice
{
	eInputDeviceKeyboard,
	eInputDeviceMouse,
	eInputDeviceGamepad,
};

enum EInputEvent
{
	eInputButtonPress, = 1,
	eInputButtonRelease = 2,
	eInputButtonHold = 3,
	eInputContinuous = 4,
	eInputEventDelta = 8,
};

enum EInputPrimitive 
{
	eInputPrimitiveInteger,
	eInputPrimitiveBoolean,
	eInputPrimitiveFloat,
	eInputPrimitiveVec2,
	eInputPrimitiveVec3,
	eInputPrimitiveVec4,
};

enum EInputProperty
{
	eInputPropertyKbdButton,
	eInputPropertyMouseDelta,
	eInputPropertyGamepadButton,
	eInputPropertyGamepadStick,
	eInputPropertyGamepadTrigger,
};

struct SInputBinding;
struct SInputBindingGroup;
struct SInputAction;

struct SInputCallbackData
{
	SInputBinding *pBinding;
	EInputProperty PropertyType;
	union {
		struct {
			EInputEvent Event;
			int Key;
		} Kbd;
		
		float MouseDelta[2];

		struct {
			EInputEvent Event;
			int Button;
		} GamepadButton;

		struct {
			float Left[2];
			float Right[2];
		} GamepadStick;

		struct {
			float Left;
			float Right;
		} GamepadTrigger;
	} Property;
};

struct SInputBinding
{
	EInputProperty PropertyType;
	union {
		int KbdButton;
		int GamepadButton;
	} Property;
};

struct SInputBindingGroup
{
	EInputDevice Device;
	TArray<SInputBinding> Bindings;
};

struct SInputAction
{
	String Name;
	EInputPrimitive PrimitiveType;
	TArray<SInputBindingGroup> Groups;
	TArray<std::function> RegisteredCallbacks;
};

struct SInputMap
{
	String Name;
	TArray<SInputAction> Actions;
};

struct CInputManager
{


protected:
	TArray<SInputMap> m_Maps;
};

#endif // _ENGINE_INPUT_MAP_H_