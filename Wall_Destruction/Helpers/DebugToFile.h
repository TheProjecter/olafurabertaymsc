#pragma once

#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <list>
using namespace std;

class DebugToFile
{
public:
	static void StartTimer();
	static void EndTimer(const char *strText, ...);
	static void Debug(const char *strText, ...);
	static bool DebugEnabled;
private:
	static std::list<clock_t> startTime;
	static ofstream file;
	static bool hasBeenSetUp;
};
