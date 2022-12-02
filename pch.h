#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
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
