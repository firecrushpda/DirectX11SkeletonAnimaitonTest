//=============================================================================
//
// 言葉処理 [StringHelper.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

class StringHelper
{
public:
	static std::wstring StringToWide(std::string str);
	static std::string GetDirectoryFromPath(const std::string & filepath);
	static std::string GetFileExtension(const std::string & filename);

	//Is contain char
	static bool IsContain(const std::string& stringBody, const char& delim);
	static bool IsContain(const std::string& stringBody, const std::string& goleStringBody);
	static bool IsContain(const std::vector<std::string>& stringContainer, const std::string& goleStringBody);

	//Split
	static void Split(const std::string& s, std::vector<std::string>& outSplitStrings, const char& delim);
	static void Split(const std::string& s, std::vector<std::string>& OutStrings, const std::string& delim);
};