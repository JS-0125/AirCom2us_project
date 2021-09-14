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

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;