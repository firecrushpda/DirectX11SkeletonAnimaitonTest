//=============================================================================
//
// ローグエラー処理 [ErrorLogger.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "COMException.h"
#include <Windows.h>

class ErrorLogger
{
public:
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
	static void Log(COMException & exception);
};