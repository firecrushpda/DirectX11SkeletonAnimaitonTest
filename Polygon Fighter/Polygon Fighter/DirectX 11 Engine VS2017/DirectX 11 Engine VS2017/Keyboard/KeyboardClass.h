//=============================================================================
//
//キーボードクラス [KeyboardClass.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "KeyboardEvent.h"
#include <queue>

class KeyboardClass
{
public:
	KeyboardClass();
	bool KeyIsPressed(const unsigned char keycode);
	bool KeyIsTriggered(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
private:
	bool autoRepeatKeys = false;//自動的にキーを繰り返し
	bool autoRepeatChars = false;//自動的に入力を繰り返し
	bool keyStates[256];//キー情報
	bool oldkeyStates[256];//キー情報
	std::queue<KeyboardEvent> keyBuffer;//キーバッファ
	std::queue<unsigned char> charBuffer;//入力バッファ
};