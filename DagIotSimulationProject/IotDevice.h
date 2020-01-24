#pragma once
#include "Tangle.h"
#include "Dag.h"
#include <mutex>

using namespace std;

class IotDevice
{
public:
	int DeviceId;
	int TransactionCounter;
	int FlushCounter;
	Dag* InternalDag;
	unsigned int MaxStorageSize;
	IotDevice(unsigned int maxStorageSize, list<Tangle*> latestFullyConfirmedTransactions, int deviceId);
	bool AddTransactionToDag(int payloadData);
	unsigned int GetCurrentSize();
private:
	std::mutex deviceMu;
	unsigned int getSizeOfDag(Dag* dag);
	bool flushToUpperLayer();
};

