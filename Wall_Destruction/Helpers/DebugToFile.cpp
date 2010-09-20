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
	if(!hasBeenSetUp)
		file.open("debug.txt", std::ios::app);

	hasBeenSetUp = true;
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

	std::string str = strBuffer;
	str.append(" in %.3f ms");

	double time = (double)(clock() - startTime.front());
	startTime.pop_front();

	PrivateEndTimer(str.c_str(), time);
	
#endif
}

void DebugToFile::PrivateEndTimer(const char *strText, ...){
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

	char timeString[9];
	_strtime(timeString);

	file << timeString << " - " << strBuffer << endl;

#endif

}

void DebugToFile::CloseFile(){
#ifdef DEBUG_TO_FILE
	if(hasBeenSetUp)
		file.close();

	hasBeenSetUp = false;
#endif
}

void DebugToFile::Debug(const char *strText, ...){
#ifdef DEBUG_TO_FILE
	if(!DebugEnabled)
		return;
	if(!hasBeenSetUp)
		file.open("debug.txt", std::ios::app);

	hasBeenSetUp = true;

	va_list args;
	char strBuffer[4096];

	if (!strText)
		return;

	va_start(args, strText);
	vsprintf_s(strBuffer, strText, args);
	va_end(args);		

	char timeString[9];
	_strtime(timeString);


	file << timeString << " - " << strBuffer << endl;
	
#endif
}