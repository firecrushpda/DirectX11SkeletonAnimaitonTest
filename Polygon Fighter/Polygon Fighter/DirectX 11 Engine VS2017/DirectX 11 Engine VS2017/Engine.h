//=============================================================================
//
// エンジン処理 [main.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "WindowContainer.h"
#include "Timer.h"


class Engine : WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
private:
	Timer timer;//タイマー

	DirectX::ContainmentType precoreslut;
};