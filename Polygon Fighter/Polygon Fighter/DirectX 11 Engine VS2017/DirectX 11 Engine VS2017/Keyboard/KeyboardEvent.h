//=============================================================================
//
//キーボード事件 [KeyboardEvent.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once

class KeyboardEvent
{
public:
	//キーボード事件種類
	enum EventType
	{
		Press,//押す
		Release,//解放
		Invalid//禁止
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	EventType type;//キーボード事件種類
	unsigned char key;//キー
};