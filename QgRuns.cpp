#include "pch.h"
#include "Resource.h"
#include "supp.h"

struct Arguments
{
	int interval;
	bool test_mode;
	bool no_check;

	std::vector<std::wstring> files;

	HINSTANCE _instance;
	std::map<int, std::wstring> _strs;

	std::vector<std::wstring> _prcs;

	Arguments(HINSTANCE instance)
		: interval(500), test_mode(false), no_check(false)
		, _instance(instance)
	{
		_strs[IDS_APP] = load_resource_string(IDS_APP, instance);
		_strs[IDS_TEST] = load_resource_string(IDS_TEST, instance);
	}

	void ParseArgs()
	{
		ParseArgs(__argc, __wargv);	// stdlib.h
	}

	void ParseArgs(int argc, wchar_t* argv[])
	{
		for (auto i = 1; i < argc; i++)
		{
			auto s = trim(argv[i]);;
			if (s.size() < 2)
				continue;
			if (!____TestOption(s))
				____AddIfExist(s);
		}
	}

	void MsgBox(int ids)
	{
		auto msg = load_resource_string(ids);
		MessageBox(NULL, msg.c_str(), _strs[IDS_APP].c_str(), MB_OK);
	}

	void MsgBox(const std::wstring& ss)
	{
		MessageBox(NULL, ss.c_str(), _strs[IDS_APP].c_str(), MB_OK);
	}

	/*private*/ void ____AddIfExist(const std::wstring& s)
	{
		if (!std::filesystem::exists(s))
			return;

		files.push_back(s);
	}

	/*private*/ bool ____TestOption(const std::wstring& s)
	{
		if (s[0] != L'-')
			return false;

		auto eq = s.find(L'=');
		auto cmd = eq != std::wstring::npos ? s.substr(1, eq - 1) : s.substr(1);

		if (cmd == L"test")
		{
			test_mode = true;
		}
		else if (cmd == L"nocheck")
		{
			no_check = true;
		}
		else if (cmd == L"interval")
		{
			if (eq == std::string::npos)
				interval = 100;
			else
			{
				auto prm = s.substr(eq + 1);
				interval = std::stoi(prm);
				if (interval < 100)
					interval = 100;
			}
		}
		else
		{
			// 알 수없는 옵션인데 패스 시킴
		}

		return true;
	}

	/*private*/ void ____ProcessList()
	{
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe = { .dwSize = sizeof(PROCESSENTRY32) };
		if (!Process32First(h, &pe))
		{
			CloseHandle(h);
			return;
		}

		do {
			_prcs.push_back(pe.szExeFile);
		} while (Process32Next(h, &pe));

		CloseHandle(h);
	}

	/*private*/ void ____ExecProc(const std::wstring& file)
	{
		if (!_prcs.empty())
		{
			auto slash = file.find_last_of(L"/\\");
			if (slash != std::wstring::npos)
			{
				auto fname = file.substr(slash + 1);
				auto it = std::find(_prcs.begin(), _prcs.end(), fname);
				if (it != _prcs.end())
					return;
			}
		}

		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi = { NULL };

		auto ret = CreateProcess(NULL, (LPWSTR)file.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (!ret)
		{
			std::wstring r(L"프로세스 실패: " + file);
			OutputDebugString(r.c_str());
		}

		Sleep(interval);
	}

	bool CanRun()
	{
		if (files.empty())
			return false;
		return true;
	}

	void Runs()
	{
		if (test_mode)
		{
			for (auto i = 0; i < files.size(); i++)
			{
				auto& f = files[i];
				const auto m{ std::format(L"{}#{}: {}", _strs[IDS_TEST], i + 1, f) };
				MsgBox(m);
			}

			return;
		}

		if (!no_check)
			____ProcessList();

		for (auto& f : files)
			____ExecProc(f);
	}
};

//
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	auto args = new Arguments(hInstance);

	args->ParseArgs();
	if (args->CanRun())
		args->Runs();
	else
		args->MsgBox(IDS_STANDALONE);

	delete args;

	return 0;
}

// 디버깅 인수:
// "D:\APPL\ksh\DarkNamer.exe" "D:\APPL\ksh\DuTools.exe" -interval=200
// "D:\FF14\act\Advanced Combat Tracker.exe" "D:\APPL\ffxiv\PrsView.exe" -interval=2000 -test
