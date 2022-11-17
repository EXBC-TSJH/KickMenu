#pragma once
#include <cstdio>
#include <ctime>
#include <windows.h>

template<typename... Args>
char MyWriteLog(const char* format, const Args&... arg)
{
	FILE* fp;
	fopen_s(&fp, "C:\\YuError.log", "a+");

	time_t tmpcal_ptr = time(0);
	tm tmp_ptr;
	errno_t time_err = gmtime_s(&tmp_ptr, &tmpcal_ptr);

	if (fp == nullptr || time_err != 0)
		return -1;

	fprintf_s(fp, "[%d:%d:%d]", tmp_ptr.tm_hour + 8, tmp_ptr.tm_min, tmp_ptr.tm_sec);
	fprintf_s(fp, format, arg...);
	fprintf_s(fp, "\n");

	fclose(fp);
	return 0;
}