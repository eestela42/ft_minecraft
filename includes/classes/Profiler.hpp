#pragma once 

#include <unordered_map>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <iomanip>

#include <lmdb.h>
#define DB_PATH "."
#define DB_NAME "Profiler.db"
#define PROFILER_ON true

class Profiler;

class ProfilerObject {
	friend Profiler;
	private:
		ProfilerObject();

		bool isTracking = false;
		const char *name;
		std::chrono::_V2::system_clock::time_point startTime;
		std::vector<std::chrono::duration<int64_t, std::nano>> data;

		ProfilerObject(const char *name);
		void StartTracking();
		void StopTracking();
		void LogData();
};

class Profiler {
	private:
		Profiler();
		static MDB_env *env;
		static MDB_dbi dbi;
		static std::unordered_map<const char *, ProfilerObject *> data;
		static bool isSaveOn;
	public:
		static void StartTracking(const char *name);
		static void StopTracking(const char *name);
		
		static void LogData();
		static void InsertData(const char *keyName, const char *keyType, const char *value);
		static char *RetrieveData(const char *keyName, const char *keyType);
		static void SetSaveOn();
};