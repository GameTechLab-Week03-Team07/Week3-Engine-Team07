#include "PlayerInput.h"

[[maybe_unused]]
static FVector GetWindowSize(HWND hWnd)
{
    RECT Rect;
    int Width = 0, Height = 0;
    if (GetClientRect(hWnd, &Rect))
    {
        Width = Rect.right - Rect.left;
        Height = Rect.bottom - Rect.top;
    }

    return FVector(Width, Height, 0.0f);
}


APlayerInput::APlayerInput()
{
	CreateKeys();

	MousePreNDCPos = FVector(0.0f, 0.0f, 0.0f);

	MousePos = FVector(0.0f, 0.0f, 0.0f);
	MousePrePos = FVector(0.0f, 0.0f, 0.0f);
	MouseNDCPos = FVector(0.0f, 0.0f, 0.0f);
}

void APlayerInput::UpdateKey(FKey& key)
{
	if (GetFocus())
	{
		if (IsKeyDown(key.KeyCode))
		{
			UpdateKeyDown(key);
		}
		else
		{
			UpdateKeyUp(key);
		}
	}
	else
	{
		ClearKeys();
	}
}

void APlayerInput::UpdateKeyDown(FKey& key) const
{
	if (key.bPressed == true)
	{
		key.KeyState = EKeyState::Press;
	}
	else
	{
		key.KeyState = EKeyState::Down;
	}

	key.bPressed = true;
}

void APlayerInput::UpdateKeyUp(FKey& key) const
{
	if (key.bPressed == true)
	{
		key.KeyState = EKeyState::Up;
	}
	else
	{
		key.KeyState = EKeyState::None;
	}

	key.bPressed = false;
}

void APlayerInput::CreateKeys()
{
	for (int vk = 0; vk <= 0xFF; ++vk)
	{
		Keys.Emplace(FKey{
			.KeyCode = static_cast<EKeyCode>(vk),
			.KeyState = EKeyState::None,
			.bPressed = false
		});
	}
}

void APlayerInput::ClearKeys()
{
	for (FKey& Key : Keys)
	{
		if (Key.KeyState == EKeyState::Down || Key.KeyState == EKeyState::Press)
		{
			Key.KeyState = EKeyState::Up;

		}
		else if (Key.KeyState == EKeyState::Up)
		{
			Key.KeyState = EKeyState::None;
		}

		Key.bPressed = false;
	}
}

bool APlayerInput::IsKeyDown(EKeyCode code) const
{
	return GetAsyncKeyState(static_cast<int>(code)) & 0x8000;
}

void APlayerInput::SetMousePos(HWND hWnd, uint32 FrameBufferWidth, uint32 FrameBufferHeight)
{
	MousePreNDCPos = MouseNDCPos;
	MousePrePos = MousePos;
	POINT Pts = {};
	GetCursorPos(&Pts);

	ScreenToClient(hWnd, &Pts);

	MousePos = FVector(Pts.x, Pts.y, 0);
	MouseNDCPos = CalNDCPos(MousePos, FVector(FrameBufferWidth, FrameBufferHeight, 0));
}

void APlayerInput::Update(HWND hWnd, uint32 FramaeBufferWidth, uint32 FramaeBufferHeight)
{
	for (FKey& key : Keys)
	{
		UpdateKey(key);
	}		
	SetMousePos(hWnd, FramaeBufferWidth, FramaeBufferHeight);


	for (const auto& [Key, Callbacks] : KeyDownCallbacks)
	{
		if (GetKeyDown(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [Key,Callbacks] : KeyPressCallbacks)
	{
		if (GetKeyPress(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [Key, Callbacks] : KeyUpCallbacks)
	{
		if (GetKeyUp(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [button, Callbacks] : MouseDownCallbacks)
	{
		if (GetKeyDown(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(MouseNDCPos);
			}
		}
	}

	for (const auto& [button, Callbacks] : MousePressCallbacks)
	{
		if (GetKeyPress(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(GetMouseDeltaPos());
			}
		}
	}

	for (const auto& [button, Callbacks] : MouseUpCallbacks)
	{
		if (GetKeyUp(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(MouseNDCPos);
			}
		}
	}

}

FVector APlayerInput::CalNDCPos(FVector InMousePos, FVector WindowSize) const
{
    return { (2.0f * InMousePos.X) / WindowSize.X - 1.0f,  (-2.0f * InMousePos.Y) / WindowSize.Y + 1.0f, 0};
}

void APlayerInput::UnregisterKeyCallbacks(uint32 UUID)
{
	// KeyPress 콜백 제거
	for (auto& [Key, Callbacks] : KeyPressCallbacks)
	{
		Callbacks.RemoveAll([UUID](const auto& CallbackInfo)
			{
				return CallbackInfo.GetID() == UUID;
			});
	}

	// KeyDown 콜백 제거
	for (auto& [Key, Callbacks] : KeyDownCallbacks)
	{
		Callbacks.RemoveAll([UUID](const auto& CallbackInfo)
			{
				return CallbackInfo.GetID() == UUID;
			});
	}

	// MousePress 콜백 제거
	for (auto& [Key, Callbacks] : MousePressCallbacks)
	{
		Callbacks.RemoveAll([UUID](const auto& CallbackInfo)
			{
				return CallbackInfo.GetID() == UUID;
			});
	}
}