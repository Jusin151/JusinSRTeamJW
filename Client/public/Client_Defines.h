#pragma once

/* 특정 헤더파일에 소속되지 않는 전역정보들을 선언해두는 역활 */
/* 기타 다른 정의파일(.h)들을 인클루드하는 역활. */
#define NOMINMAX
#define NOCRYPT
#include <Windows.h>
#include <process.h>

//// 매크로 강제 해제
//#undef min
//#undef max
#include "Client_Enum.h"
#include "unordered_map"
#include <algorithm>
using namespace std;



namespace Client
{
	//기본 1280 720
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;
}

extern HWND g_hWnd;


using namespace Client;