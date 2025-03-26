#pragma once
#include <functional>

#include "Core/AbstractClass/Singleton.h"
#include "Core/Math/Vector.h"
#include "Core/Container/Map.h"
#include "Core/Container/Array.h"


/**
 * 윈도우 키 코드 열거형
 * @note https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
 */
enum class EKeyCode : uint8
{
	// 마우스 버튼
	LButton = 0x01,
	RButton = 0x02,
	MButton = 0x04,

	// 키보드 키
	Backspace = 0x08,
    Tab = 0x09,
    Enter = 0x0D,
    Shift = 0x10,
    Ctrl = 0x11,
    Control = 0x11,
    Alt = 0x12,
    Pause = 0x13,
    CapsLock = 0x14,
    Esc = 0x1B,
    Escape = 0x1B,
    Space = 0x20,
    PageUp = 0x21,
    PageDown = 0x22,
    End = 0x23,
    Home = 0x24,
    Left = 0x25,
    Up = 0x26,
    Right = 0x27,
    Down = 0x28,
    SnapShot = 0x2C,
    PrintScreen = 0x2C,
    Insert = 0x2D,
    Delete = 0x2E,
    _0 = 0x30,
    _1 = 0x31,
    _2 = 0x32,
    _3 = 0x33,
    _4 = 0x34,
    _5 = 0x35,
    _6 = 0x36,
    _7 = 0x37,
    _8 = 0x38,
    _9 = 0x39,
    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4A,
    K = 0x4B,
    L = 0x4C,
    M = 0x4D,
    N = 0x4E,
    O = 0x4F,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5A,
    Num0 = 0x60,
    Num1 = 0x61,
    Num2 = 0x62,
    Num3 = 0x63,
    Num4 = 0x64,
    Num5 = 0x65,
    Num6 = 0x66,
    Num7 = 0x67,
    Num8 = 0x68,
    Num9 = 0x69,
    NumMul = 0x6A,
    NumAdd = 0x6B,
    NumSub = 0x6D,
    NumDot = 0x6E,
    NumDiv = 0x6F,
    F1 = 0x70,
    F2 = 0x71,
    F3 = 0x72,
    F4 = 0x73,
    F5 = 0x74,
    F6 = 0x75,
    F7 = 0x76,
    F8 = 0x77,
    F9 = 0x78,
    F10 = 0x79,
    F11 = 0x7A,
    F12 = 0x7B,
    F13 = 0x7C,
    F14 = 0x7D,
    F15 = 0x7E,
    F16 = 0x7F,
    F17 = 0x80,
    F18 = 0x81,
    F19 = 0x82,
    F20 = 0x83,
    F21 = 0x84,
    F22 = 0x85,
    F23 = 0x86,
    F24 = 0x87,
    NumLock = 0x90,
    ScrollLock = 0x91,
    LShift = 0xA0,
	LeftShift = 0xA0,
    RShift = 0xA1,
	RightShift = 0xA1,
    LControl = 0xA2,
	LeftControl = 0xA2,
    RControl = 0xA3,
	RightControl = 0xA3,
    LAlt = 0xA4,
	LeftAlt = 0xA4,
    RAlt = 0xA5,
	RightAlt = 0xA5,
};

enum class EKeyState : uint8
{
	None,
	Down,  // 버튼이 눌린 경우 (매 틱마다)
	Press, // 버튼이 눌린 경우 (1틱만)
	Up,    // 버튼이 때진 경우 (매 틱마다)
};

class KeyCallbackWrapper
{
public:
	KeyCallbackWrapper(const std::function<void()>& Callback, uint32 uuid) : Callback(Callback), ID(uuid) {}

	void operator()() const { Callback(); }

	uint32 GetID() const { return ID; }

private:
	std::function<void()> Callback;
	uint32 ID;
};

class MouseCallbackWrapper
{
public:
	MouseCallbackWrapper(const std::function<void(const FVector&)>& Callback, uint32 uuid) : Callback(Callback), ID(uuid) {}

	void operator()(const FVector& MousePos) const { Callback(MousePos); }

	uint32 GetID() const { return ID; }
private:
	std::function<void(const FVector&)> Callback;
	uint32 ID;
};

class APlayerInput : public TSingleton<APlayerInput>
{
public:
	struct FKey
	{
		EKeyCode KeyCode;
		EKeyState KeyState;
		bool bPressed;
	};
    APlayerInput();

	inline bool GetKeyDown(EKeyCode key) const { return Keys[static_cast<uint8>(key)].KeyState == EKeyState::Down; }

	inline bool GetKeyPress(EKeyCode key) const { return Keys[static_cast<uint8>(key)].KeyState == EKeyState::Press; }

	inline bool GetKeyUp(EKeyCode key) const { return Keys[static_cast<uint8>(key)].KeyState == EKeyState::Up; }

	void Update(HWND hWnd, uint32 FramaeBufferHeight, uint32 FramaeBufferWidth);
    
    FVector GetMousePos() const { return MousePos;}
    FVector GetMouseNDCPos() const { return MouseNDCPos;}
	FVector GetMouseDeltaPos() const { return MouseNDCPos - MousePreNDCPos; }
	float GetMouseMovementDistance() const { return GetMouseDeltaPos().Length(); }
	FVector GetMouseScreenDeltaPos() const { return (MousePos - MousePrePos); }

	template <typename Fn>
	void RegisterKeyDownCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid);
	template <typename Fn>
	void RegisterKeyPressCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid);
	template <typename Fn>
	void RegisterKeyUpCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid);

	template <typename Fn>
	void RegisterMouseDownCallback(EKeyCode Button, const Fn& Callback, uint32 uuid);
	template <typename Fn>
	void RegisterMousePressCallback(EKeyCode Button, const Fn& Callback, uint32 uuid);
	template <typename Fn>
	void RegisterMouseUpCallback(EKeyCode Button, const Fn& Callback, uint32 uuid);
	void UnregisterKeyCallbacks(uint32 UUID);

private:
	void CreateKeys();
	void ClearKeys();

	bool IsKeyDown(EKeyCode code) const;

	void UpdateKey(FKey& key);
	void UpdateKeyDown(FKey& key) const;
	void UpdateKeyUp(FKey& key) const;

	void SetMousePos(HWND hWnd, uint32 FrameBufferWidth, uint32 FrameBufferHeight);

	FVector CalNDCPos(FVector InMousePos, FVector WindowSize) const;
private:
	// Key 이벤트에 대한 콜백들을 저장하는 맵 (각 키마다 여러 콜백을 가질 수 있음)
	TMap<EKeyCode, TArray<KeyCallbackWrapper>> KeyDownCallbacks;
	TMap<EKeyCode, TArray<KeyCallbackWrapper>> KeyPressCallbacks;
	TMap<EKeyCode, TArray<KeyCallbackWrapper>> KeyUpCallbacks;

	// 마우스 이벤트에 대한 콜백들을 저장 (button 0: 좌클릭, 1: 우클릭)
	TMap<EKeyCode, TArray<MouseCallbackWrapper>> MouseDownCallbacks;
	TMap<EKeyCode, TArray<MouseCallbackWrapper>> MousePressCallbacks;
	TMap<EKeyCode, TArray<MouseCallbackWrapper>> MouseUpCallbacks;

	TArray<FKey> Keys;

	[[maybe_unused]]
    bool bIsBlockInput = false;

    FVector MousePreNDCPos;
    FVector MousePos;
    FVector MousePrePos;
    FVector MouseNDCPos;
};

template <typename Fn>
void APlayerInput::RegisterKeyDownCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid)
{
	if (KeyDownCallbacks.Contains(KeyCode))
	{
		for (const auto& Wrapper : KeyDownCallbacks[KeyCode])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	KeyDownCallbacks[KeyCode].Emplace(Callback, uuid);
}

template <typename Fn>
void APlayerInput::RegisterKeyPressCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid)
{
	if (KeyPressCallbacks.Contains(KeyCode))
	{
		for (const auto& Wrapper : KeyPressCallbacks[KeyCode])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	KeyPressCallbacks[KeyCode].Emplace(Callback, uuid);
}

template <typename Fn>
void APlayerInput::RegisterKeyUpCallback(EKeyCode KeyCode, const Fn& Callback, uint32 uuid)
{
	if (KeyUpCallbacks.Contains(KeyCode))
	{
		for (const auto& Wrapper : KeyUpCallbacks[KeyCode])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	KeyUpCallbacks[KeyCode].Emplace(Callback, uuid);
}

template <typename Fn>
void APlayerInput::RegisterMouseDownCallback(EKeyCode Button, const Fn& Callback, uint32 uuid)
{
	//if (Button != EKeyCode::LButton || Button != EKeyCode::MButton || Button != EKeyCode::RButton)
	//{
	//	return;
	//}

	if (MouseDownCallbacks.Contains(Button))
	{
		for (const auto& Wrapper : MouseDownCallbacks[Button])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	MouseDownCallbacks[Button].Emplace(Callback, uuid);
}

template <typename Fn>
void APlayerInput::RegisterMousePressCallback(EKeyCode Button, const Fn& Callback, uint32 uuid)
{
	//if (Button != EKeyCode::LButton || Button != EKeyCode::MButton || Button != EKeyCode::RButton)
	//{
	//	return;
	//}

	if (MousePressCallbacks.Contains(Button))
	{
		for (const auto& Wrapper : MousePressCallbacks[Button])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	MousePressCallbacks[Button].Emplace(Callback, uuid);
}

template <typename Fn>
void APlayerInput::RegisterMouseUpCallback(EKeyCode Button, const Fn& Callback, uint32 uuid)
{
	//if (Button != EKeyCode::LButton || Button != EKeyCode::MButton || Button != EKeyCode::RButton)
	//{
	//	return;
	//}

	if (MouseUpCallbacks.Contains(Button))
	{
		for (const auto& Wrapper : MouseUpCallbacks[Button])
		{
			if (Wrapper.GetID() == uuid)
			{
				return;
			}
		}
	}

	MouseUpCallbacks[Button].Emplace(Callback, uuid);
}

