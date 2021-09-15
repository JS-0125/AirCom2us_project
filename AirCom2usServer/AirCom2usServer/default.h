#pragma once
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <array>
#include <mutex>
#include<queue>
#include <chrono>
#include<string>

extern "C" {
#include"lua.h"
#include "lauxlib.h"
#include"lualib.h"
}

#pragma comment( lib, "lua54.lib" )
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;