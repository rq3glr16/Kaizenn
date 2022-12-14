#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <intrin.h>
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <thread>
#include <vector>
#include <fstream>
#include <shlobj.h>

// D3d Related Files
#include "ImGui/imgui.h"
#include "imgui_impl_dx11.h"
#include <DXGI.h>
#include <d3d11.h>
//#pragma comment(lib, "Flax.lib")
//#include "Flax/Fiber.h"

#include "fmt\format.h"
#include "fmt\color.h"

#include "Helpers\HiddenText.h"
#include "Helpers\Scan.h"
#include "Helpers\Call.h"

using namespace std;