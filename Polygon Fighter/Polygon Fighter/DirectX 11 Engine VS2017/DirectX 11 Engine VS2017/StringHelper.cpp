//=============================================================================
//
// 言葉処理 [StringHelper.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "StringHelper.h"

std::wstring StringHelper::StringToWide(std::string str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::GetDirectoryFromPath(const std::string & filepath)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
	{
		return "";
	}
	if (off1 == std::string::npos)
	{
		return filepath.substr(0, off2);
	}
	if (off2 == std::string::npos)
	{
		return filepath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filepath.substr(0, std::max(off1, off2));
}

std::string StringHelper::GetFileExtension(const std::string & filename)
{
	size_t off = filename.find_last_of('.');
	if (off == std::string::npos)
	{
		return {};
	}
	return std::string(filename.substr(off + 1));
}

bool StringHelper::IsContain(const std::string& stringBody, const char& delim)
{
	bool Ret = false;

	std::string::size_type idx = stringBody.find(delim);
	if (idx != std::string::npos)
		Ret = true;
	else
		Ret = false;

	return Ret;
}

bool StringHelper::IsContain(const std::string& stringBody, const std::string& goleStringBody)
{
	bool Ret = false;

	if (stringBody.find(goleStringBody) != std::string::npos)
		Ret = true;

	return Ret;
}

bool StringHelper::IsContain(const std::vector<std::string>& stringContainer, const std::string& goleStringBody)
{
	for (const std::string& data : stringContainer)
	{
		if (data == goleStringBody)
			return true;
	}
	return false;
}

void StringHelper::Split(
	const std::string& s,
	std::vector<std::string>& outSplitStrings,
	const char& delim
)
{
	std::istringstream iss(s);
	std::string temp;

	temp.clear();
	while (std::getline(iss, temp, delim))
	{
		outSplitStrings.emplace_back(std::move(temp));
	}

	return;
}

void StringHelper::Split(const std::string& s, std::vector<std::string>& OutStrings, const std::string& delim)
{
	std::string::size_type pos1, pos2;
	size_t len = s.length();
	pos2 = s.find(delim);
	pos1 = 0;

	while (std::string::npos != pos2)
	{
		OutStrings.emplace_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + delim.size();
		pos2 = s.find(delim, pos1);
	}

	if (pos1 != len)
		OutStrings.emplace_back(s.substr(pos1));
}
