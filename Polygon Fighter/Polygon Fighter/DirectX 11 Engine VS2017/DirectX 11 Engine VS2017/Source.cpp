//=============================================================================
//
// ソース処理 [main.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Engine.h"

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	// ウィンドウの作成
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600))
	{
		// メッセージループ
		while (engine.ProcessMessages() == true)
		{
			engine.Update();		// 更新処理
			engine.RenderFrame();	// 描画処理
		}
	}
	return 0;
}