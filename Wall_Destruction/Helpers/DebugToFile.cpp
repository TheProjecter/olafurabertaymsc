#include "DebugToFile.h"
#include "time.h"
#include <stdarg.h>

std::list<clock_t> DebugToFile::startTime;
ofstream DebugToFile::file;
bool DebugToFile::hasBeenSetUp = false;
bool DebugToFile::DebugEnabled = true;

void DebugToFile::StartTimer(){
#ifdef DEBUG_TO_FILE
	if(!DebugEnabled)
		return;
	startTime.push_back(clock());
#endif
}

void DebugToFile::EndTimer(const char *strText, ...){
#ifdef DEBUG_TO_FILE
	if(!DebugEnabled)
		return;

	va_list args;
	char strBuffer[4096];

	if (!strText)
		return;

	va_start(args, strText);
	vsprintf_s(strBuffer, strText, args);
	va_end(args);		


	file.open("debug.txt", std::ios::app);

	char timeString[9];
	_strtime(timeString);

	double time = (double)(clock() - startTime.back())/ (double)CLOCKS_PER_SEC;
	startTime.pop_back();

	file << timeString << " - " << strBuffer << " : " << time << endl;
	file.close();

#endif
}

void DebugToFile::Debug(const char *strText, ...){
#ifdef DEBUG_TO_FILE
	if(!DebugEnabled)
		return;
	va_list args;
	char strBuffer[4096];

	if (!strText)
		return;

	va_start(args, strText);
	vsprintf_s(strBuffer, strText, args);
	va_end(args);		


	file.open("debug.txt", std::ios::app);

	char timeString[9];
	_strtime(timeString);


	file << timeString << " - " << strBuffer << endl;
	file.close();
#endif
}