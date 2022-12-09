#include "pch.h"
#include "Resource.h"
#include "supp.h"

enum Commands
{
	CmdNone,
	CmdRun,
	CmdCopy,
	CmdMove,
	CmdMaxValue,
};

const wchar_t* c_command_string[] =
{
	L"NONE",
	L"RUN",
	L"COPY",
	L"MOVE",
	L"알 수 없는 명령",
};

struct Arguments
{
	Commands command;
	int interval;
	bool test_mode;
	bool no_check;
	bool silence;

	std::vector<std::wstring> files;

	HINSTANCE _instance;
	std::map<int, std::wstring> _strs;

	std::vector<std::wstring> _prcs;

	explicit Arguments(const HINSTANCE instance)
		: command(CmdNone), interval(500), test_mode(false), no_check(false), silence(false)
		, _instance(instance)
	{
		_strs[IDS_APP] = load_resource_string(IDS_APP, instance);
		_strs[IDS_TEST] = load_resource_string(IDS_TEST, instance);
		_strs[IDS_CURMODE] = load_resource_string(IDS_CURMODE, instance);
	}

	void ParseArgs()
	{
		ParseArgs(__argc, __wargv);	// stdlib.h
	}

	void ParseArgs(const int argc, wchar_t* argv[])
	{
		if (argc < 2)
			return;

		auto start = 2;
		if (!_test_command(argv[1]))
			start = 1;

		for (auto i = start; i < argc; i++)
		{
			auto s = trim_string(argv[i]);
			if (s.size() < 2)
				continue;
			if (!_test_option(s))
				_add_file(s);
		}
	}

	void MsgBox(const int ids)
	{
		if (silence)
			return;
		const auto msg = load_resource_string(ids);
		MessageBox(nullptr, msg.c_str(), _strs[IDS_APP].c_str(), MB_OK);
	}

	void MsgBox(const std::wstring& ss)
	{
		if (silence)
			return;
		MessageBox(nullptr, ss.c_str(), _strs[IDS_APP].c_str(), MB_OK);
	}

	/*private*/ void _add_file(const std::wstring& s)
	{
		if (command == CmdRun && !std::filesystem::exists(s))
			return;

		files.push_back(s);
	}

	/*private*/ bool _test_option(const std::wstring& s)
	{
		if (s[0] != L'-')
			return false;

		const auto eq = s.find(L'=');
		const auto cmd = eq != std::wstring::npos ? s.substr(1, eq - 1) : s.substr(1);

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
				const auto prm = s.substr(eq + 1);
				interval = std::stoi(prm);
				if (interval < 100)
					interval = 100;
			}
		}
		else if (cmd == L"silence")
		{
			silence = true;
		}
		else
		{
			// 알 수없는 옵션인데 패스 시킴
		}

		return true;
	}

	/*private*/ bool _test_command(const std::wstring& s)
	{
		const auto cmd = upper_string(s);

		if (cmd == c_command_string[CmdRun])
		{
			command = CmdRun;
			return true;
		}
		if (cmd == c_command_string[CmdCopy])
		{
			command = CmdCopy;
			return true;
		}
		if (cmd == c_command_string[CmdMove])
		{
			command = CmdMove;
			return true;
		}

		command = CmdRun;
		return false;
	}

	/*private*/ void _process_list()
	{
		const HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe = { .dwSize = sizeof(PROCESSENTRY32) };
		if (!Process32First(h, &pe))
		{
			CloseHandle(h);
			return;
		}

		do {
			_prcs.emplace_back(pe.szExeFile);
		} while (Process32Next(h, &pe));

		CloseHandle(h);
	}

	/*private*/ void _exec_process(const std::wstring& file)
	{
		if (!_prcs.empty())
		{
			if (const auto slash = file.find_last_of(L"/\\");
				slash != std::wstring::npos)
			{
				const auto fname = file.substr(slash + 1);
				if (const auto it = std::ranges::find(_prcs, fname);
					it != _prcs.end())
					return;
			}
		}

		STARTUPINFO si = { sizeof(STARTUPINFO), };
		PROCESS_INFORMATION pi = { nullptr, };

		const auto ret = CreateProcess(nullptr, const_cast<LPWSTR>(file.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (!ret)
		{
			const std::wstring r(L"프로세스 실패: " + file);
			OutputDebugString(r.c_str());
		}

		Sleep(interval);
	}

	/*private*/ void _do_run()
	{
		if (!no_check)
			_process_list();

		for (const auto& f : files)
			_exec_process(f);
	}

	/*private*/ void _do_copy_move() const
	{
		if (files.size() != 2)
			return;

		const auto src = files[0].c_str();
		const auto dst = files[1].c_str();

		if (!std::filesystem::exists(src))
			return;

		if (command == CmdCopy)
		{
			// 복사
			if (!CopyFile(src, dst, FALSE))
			{
				const auto m{ std::format(L"파일 복사 실패({}): {}", GetLastError(), src) };
				OutputDebugString(m.c_str());
			}
		}
		else
		{
			// 이동
			constexpr DWORD flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING;
			MoveFileEx(src, dst, flags);
		}
	}

	void LetsDoIt()
	{
		if (test_mode)
		{
			auto cmd = c_command_string[command < CmdMaxValue ? command : CmdMaxValue];
			const auto cm{ std::format(L"{}: {}", _strs[IDS_CURMODE], cmd) };
			MsgBox(cm);

			auto n = 0;
			for (auto& i : files)
			{
				const auto m{ std::format(L"{}#{}: {}", _strs[IDS_TEST], ++n, i) };
				MsgBox(m);

			}
			return;
		}

		switch (command)  // NOLINT(clang-diagnostic-switch-enum)
		{
		case CmdRun: _do_run(); break;
		case CmdCopy:
		case CmdMove: _do_copy_move(); break;
		default: break;
		}
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

	const auto args = new Arguments(hInstance);

	args->ParseArgs();
	if (args->command == CmdNone)
		args->MsgBox(IDS_STANDALONE);
	else
		args->LetsDoIt();

	delete args;

	return 0;
}

// 디버깅 인수:
// "D:\APPL\ksh\DarkNamer.exe" "D:\APPL\ksh\DuTools.exe" -interval=200
// run "D:\FF14\act\Advanced Combat Tracker.exe" "D:\APPL\ffxiv\PrsView.exe" -interval=2000 -test
// copy "D:\APPL\ksh\pwall.avi" "Z:\pwallcopy.avi" -silence
