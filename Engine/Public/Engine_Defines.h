#pragma once

#pragma warning (disable : 4251)
#define NOMINMAX
#include <d3d9.h>
#include <d3dx9.h>

#include <string>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <shobjidl.h> // IFileOpenDialog 사용을 위한 헤더
#include <list>
#include <vector>
#include <fstream>
#include <set>
#include <codecvt>
#include <locale>
using namespace std;

#include "../../External Libraries/json.hpp"
#include "Engine_Struct.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Macro.h"

using namespace Engine;
using json = nlohmann::json;
#pragma region FMOD
#include "fmod.h"
#include "fmod_studio.hpp"

using namespace FMOD;
#pragma endregion
#pragma region IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "../../External Libraries/ImGuizmo-master/ImGuizmo.h" 
#pragma endregion

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif



