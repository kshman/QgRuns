#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//
#include <TlHelp32.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")

//
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>
#include <regex>
#include <format>
#include <map>
