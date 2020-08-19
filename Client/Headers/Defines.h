#pragma once

//	C++ Standard Libraries & Standard Template Libraries
#include <iostream>
#include <fstream>
#include <array>
#include <queue>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>
#include <ctime>
#include <process.h>

#include <chrono>
#include <thread>
#include <condition_variable>
#include <cstdlib>
#include <math.h>

//	easy_profiler

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include <easy/arbitrary_value.h>
#include <easy/reader.h>

using namespace std;

//	Direct3D
#include <d3d9.h>
#include <d3dx9.h>

//	XInput
#define	DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

//	pragma warning
#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

//	Custom
#include "Macro.h"
#include "Typedef.h"
#include "Function.h"
#include "Struct.h"
#include "Global.h"

////	Only Debug
//#ifdef _DEBUG
//
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//#endif 

//#endif