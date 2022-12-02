#pragma once

// ¹®ÀÚ¿­
/*
static inline std::wstring trim_rex(const std::wstring& s)
{
	// left: L"^\\s+"
	// right: L"\\s+$"
	return std::regex_replace(s, std::wregex(L"^(\\s+)|(\\s+)$"), std::wstring(L""));
}
*/

static const std::wstring WHITESPACE = L" \r\n\t\f\v";

static inline std::wstring trim_left(const std::wstring& s)
{
	auto p = s.find_first_not_of(WHITESPACE);
	return (p == std::wstring::npos) ? L"" : s.substr(p);
}

static inline std::wstring trim_right(const std::wstring& s)
{
	auto p = s.find_last_not_of(WHITESPACE);
	return (p == std::wstring::npos) ? L"" : s.substr(0, p + 1);
}

static inline std::wstring trim(const std::wstring& s)
{
	return trim_left(trim_right(s));
}

static inline std::wstring load_resource_string(int ids, HINSTANCE instance = NULL)
{
	const wchar_t* p;
	auto l = LoadString(instance, ids, (LPWSTR)&p, 0);
	return l > 0 ? std::wstring(p, l) : std::wstring();
}
