//=============================================================================
//
// ウインドウズ処理 [WindowContainer.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics/Graphics.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window; //レンダリングウインドウ
	KeyboardClass keyboard;//キーボード
	MouseClass mouse;//マウス
	Graphics gfx;//グラフィックス
private:
};