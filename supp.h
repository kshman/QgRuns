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

inline std::wstring trim_left(const std::wstring& s)
{
	auto p = s.find_first_not_of(WHITESPACE);
	return (p == std::wstring::npos) ? L"" : s.substr(p);
}

inline std::wstring trim_right(const std::wstring& s)
{
	auto p = s.find_last_not_of(WHITESPACE);
	return (p == std::wstring::npos) ? L"" : s.substr(0, p + 1);
}

inline std::wstring trim_string(const std::wstring& s)
{
	return trim_left(trim_right(s));
}

inline std::wstring upper_string(const std::wstring& s)
{
	std::wstring r(s);
	std::ranges::transform(r, r.begin(), towupper);
	return r;
}

inline std::wstring load_resource_string(const int ids, const HINSTANCE instance = nullptr)
{
	const wchar_t* p;
	auto l = LoadString(instance, ids, reinterpret_cast<LPWSTR>(&p), 0);
	return l > 0 ? std::wstring(p, l) : std::wstring();
}
