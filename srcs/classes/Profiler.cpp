#include <classes/Profiler.hpp>

/* *************** ProfilerObject class **************** */

ProfilerObject::ProfilerObject(const char *name) : name(name) {

}

void ProfilerObject::StartTracking() {
	if (!isTracking) {
		startTime = std::chrono::high_resolution_clock::now();
		isTracking = true;
	}
}

void ProfilerObject::StopTracking() {
	if (isTracking) {
		data.push_back(std::chrono::high_resolution_clock::now() - startTime);
		isTracking = false;
	}
}

void ProfilerObject::LogData() {
	std::chrono::nanoseconds average = (std::chrono::nanoseconds)0;
	std::chrono::nanoseconds median;
	std::chrono::nanoseconds worst;
	std::chrono::nanoseconds worst5percent;
	std::chrono::nanoseconds best;
	std::chrono::nanoseconds best5percent;
	char *value;
	const char *sign;
	std::chrono::nanoseconds dbValue;
	double resultValue;

	if (!data.size()) {
		return ;
	}
	sort(data.begin(), data.end());
	if (data.size() % 2) {
		median = data[data.size() / 2];
	}
	else {
		median = (data[data.size() / 2 - 1] + data[data.size() / 2]) / 2;
	}
	worst = data[data.size() - 1];
	worst5percent = data[(float)data.size() / 100 * 95];
	best = data[0];
	best5percent = data[(float)data.size() / 100 * 5];
	for (auto const& value : data)
	{
		average += value;
	}
	average /= data.size();

	
	const char *unit;
	double divider;
	if (worst.count() < 1000) {
		unit = " ns";
		divider = 1;
	}
	else if (worst.count() < 1000000) {
		unit = " μs";
		divider = 1000;
	}
	else if (worst.count() < 1000000000) {
		unit = " ms";
		divider = 1000000;
	}
	else {
		unit = " s";
		divider = 1000000000;
	}

	//std::cout.setf(std::ios::fixed, std::ios::floatfield);
	//std::cout.precision(4);
	//std::cout.width(5);
	std::cout << std::showpoint;
	std::cout.precision(4);
	//std::cout.width(5);
	//
	std::cout << "---------- " << name << " ----------" << std::endl;
	if (data.size() == 1) {
		if ((value = Profiler::RetrieveData(name, "Time"))) {
			dbValue = std::chrono::nanoseconds(atol(value));
			resultValue = (best < dbValue) ? (1 - (double)best.count() / (double)dbValue.count()) * 100: ((double)best.count() / (double)dbValue.count() - 1) * 100;
			sign = (best < dbValue) ? " -": " +";
			std::cout << "Time     : " << best.count() / divider << unit << sign << resultValue << "%"<< std::endl;
		}
		else {
			std::cout << "Time     : " << best.count() / divider << unit << std::endl;
		}
		Profiler::InsertData(name, "Time", std::to_string(best.count()).c_str());
		return ;
	}

	if ((value = Profiler::RetrieveData(name, "Average"))) {
		dbValue = std::chrono::nanoseconds(atol(value));
		resultValue = (average < dbValue) ? (1 - (double)average.count() / (double)dbValue.count()) * 100: ((double)average.count() / (double)dbValue.count() - 1) * 100;
		sign = (average < dbValue) ? " -": " +";
		std::cout << "Average  : " << average.count() / divider << unit << sign << resultValue << "%"<< std::endl;
	}
	else {
		std::cout << "Average  : " << average.count() / divider << unit << std::endl;
	}

	if ((value = Profiler::RetrieveData(name, "Median"))) {
		dbValue = std::chrono::nanoseconds(atol(value));
		resultValue = (median < dbValue) ? (1 - (double)median.count() / (double)dbValue.count()) * 100: ((double)median.count() / (double)dbValue.count() - 1) * 100;
		sign = (median < dbValue) ? " -": " +";
		std::cout << "Median   : " << median.count() / divider << unit << sign << resultValue << "%"<< std::endl;
	}
	else {
		std::cout << "Median   : " << median.count() / divider << unit << std::endl;
	}

	if ((value = Profiler::RetrieveData(name, "Worst"))) {
		dbValue = std::chrono::nanoseconds(atol(value));
		resultValue = (worst < dbValue) ? (1 - (double)worst.count() / (double)dbValue.count()) * 100: ((double)worst.count() / (double)dbValue.count() - 1) * 100;
		sign = (worst < dbValue) ? " -": " +";
		std::cout << "Worst    : " << worst.count() / divider << unit << sign << resultValue << "%"<< std::endl;
	}
	else {
		std::cout << "Worst    : " << worst.count() / divider << unit << std::endl;
	}

	if (data.size() >= 40) {
		if ((value = Profiler::RetrieveData(name, "Worst5%"))) {
			dbValue = std::chrono::nanoseconds(atol(value));
			resultValue = (worst5percent < dbValue) ? (1 - (double)worst5percent.count() / (double)dbValue.count()) * 100: ((double)worst5percent.count() / (double)dbValue.count() - 1) * 100;
			sign = (worst5percent < dbValue) ? " -": " +";
			std::cout << "Worst 5% : " << worst5percent.count() / divider << unit << sign << resultValue << "%"<< std::endl;
		}
		else {
			std::cout << "Worst 5% : " << worst5percent.count() / divider << unit << std::endl;
		}
	}

	if ((value = Profiler::RetrieveData(name, "Best"))) {
		dbValue = std::chrono::nanoseconds(atol(value));
		resultValue = (best < dbValue) ? (1 - (double)best.count() / (double)dbValue.count()) * 100: ((double)best.count() / (double)dbValue.count() - 1) * 100;
		sign = (best < dbValue) ? " -": " +";
		std::cout << "Best     : " << best.count() / divider << unit << sign << resultValue << "%"<< std::endl;
	}
	else {
		std::cout << "Best     : " << best.count() / divider << unit << std::endl;
	}

	if (data.size() >= 40) {
		if ((value = Profiler::RetrieveData(name, "Best5%"))) {
			dbValue = std::chrono::nanoseconds(atol(value));
			resultValue = (best5percent < dbValue) ? (1 - (double)best5percent.count() / (double)dbValue.count()) * 100: ((double)best5percent.count() / (double)dbValue.count() - 1) * 100;
			sign = (best5percent < dbValue) ? " -": " +";
			std::cout << "Best  5% : " << best5percent.count() / divider << unit << sign << resultValue << "%"<< std::endl;
		}
		else {
			std::cout << "Best  5% : " << best5percent.count() / divider << unit << std::endl;
		}
	}

	//Write Data to db
	Profiler::InsertData(name, "Average", std::to_string(average.count()).c_str());
	Profiler::InsertData(name, "Median", std::to_string(median.count()).c_str());
	Profiler::InsertData(name, "Worst", std::to_string(worst.count()).c_str());
	Profiler::InsertData(name, "Best", std::to_string(best.count()).c_str());
	if (data.size() >= 40) {
		Profiler::InsertData(name, "Best5%", std::to_string(best5percent.count()).c_str());
		Profiler::InsertData(name, "Worst5%", std::to_string(worst5percent.count()).c_str());
	}
}




/* *************** Profiler class **************** */

std::unordered_map<const char *, ProfilerObject *> Profiler::data;
bool Profiler::isSaveOn = false;
MDB_env *Profiler::env;
MDB_dbi Profiler::dbi;

void Profiler::StartTracking(const char *name) {
	if (data.find(name) == data.end()) {
		data[name] = new ProfilerObject(name);
		
	}
	data[name]->StartTracking();
}

void Profiler::StopTracking(const char *name) {
	if (data.find(name) != data.end()) {
		data[name]->StopTracking();
	}
}

void Profiler::LogData() {
	std::ofstream out("Profiler.log");
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());
	MDB_txn *txn;

	int rc = mdb_env_create(&env);
    if (rc != 0) assert(!"LMDB env create didn't work");
	rc = mdb_env_open(env, DB_PATH, 0, 0664);
	if (rc != 0) assert(!"LMDB env open didn't work");
	rc = mdb_txn_begin(env, NULL, 0, &txn);
	if (rc != 0) assert(!"LMDB transaction begin didn't work");
	rc = mdb_dbi_open(txn, 0, 0, &dbi);
    if (rc != 0) assert(!"LMDB database open didn't work");
	mdb_txn_abort(txn);

	for (auto const& keyValue : data)
	{
		keyValue.second->LogData();
	}

	mdb_dbi_close(env, dbi);
    mdb_env_close(env);

	std::cout.rdbuf(coutbuf);
}

void Profiler::InsertData(const char *keyName, const char *keyType, const char *value) {
	char key[256];
    MDB_txn *txn;

	if (!isSaveOn) return ;

	snprintf(key, sizeof(key), "%s%s", keyName, keyType);
    int rc = mdb_txn_begin(env, NULL, 0, &txn);
    if (rc != 0) assert(!"LMDB insert transaction begin didn't work");
	
    MDB_val mdbKey = { strlen(key), (void *)key };
    MDB_val mdbValue = { strlen(value), (void *)value };

    rc = mdb_put(txn, dbi, &mdbKey, &mdbValue, 0);

    if (rc != 0) {
        mdb_txn_abort(txn);
        assert(!"LMDB put didn't work");
    }

    rc = mdb_txn_commit(txn);
    if (rc != 0) assert(!"LMDB insert transaction commit didn't work");
}

char *Profiler::RetrieveData(const char *keyName, const char *keyType) {
	char key[256];
	char *ret = NULL;
    MDB_txn *txn;

	snprintf(key, sizeof(key), "%s%s", keyName, keyType);
    int rc = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
    if (rc != 0) assert(!"LMDB retrieve transaction begin didn't work");

    MDB_val mdbKey = { strlen(key), (void *)key };
    MDB_val mdbValue;

    rc = mdb_get(txn, dbi, &mdbKey, &mdbValue);

	if (!rc) {
		ret = strndup((char*)mdbValue.mv_data, mdbValue.mv_size);
	}
	else if (rc == MDB_NOTFOUND) {
        printf("[%s] Key not found in LMDB\n", key);
    } else {
        fprintf(stderr, "Error retrieving data from LMDB: %s\n", mdb_strerror(rc));
    }

    mdb_txn_abort(txn);
	return ret;
}

void Profiler::SetSaveOn() {
	isSaveOn = true;
}