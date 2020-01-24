#include "IotDevice.h"

IotDevice::IotDevice(unsigned int maxStorageSize, list<Tangle*> latestFullyConfirmedTransactions, int deviceId)
{
	this->MaxStorageSize = maxStorageSize;
	this->InternalDag = new Dag();
	this->InternalDag->Tangles = latestFullyConfirmedTransactions;
	this->DeviceId = deviceId;

	this->TransactionCounter = 0;
	this->FlushCounter = 0;
}

bool IotDevice::AddTransactionToDag(int payloadData)
{
	deviceMu.lock();

	this->TransactionCounter++;
	if (GetCurrentSize() + sizeof(payloadData) > MaxStorageSize) {
		flushToUpperLayer();
		this->FlushCounter++;
		Tangle* tangle = new Tangle();
		tangle->PayloadData = payloadData;
		tangle->confirmationLevel = 0;
		this->InternalDag->Tangles.push_back(tangle);
		deviceMu.unlock();

		return false;
	}
	Tangle* tangle = new Tangle();
	tangle->PayloadData = payloadData;
	tangle->confirmationLevel = 0;
	this->InternalDag->Tangles.push_back(tangle);
	deviceMu.unlock();
	return true;
}

unsigned int IotDevice::GetCurrentSize()
{
	return getSizeOfDag(this->InternalDag);
}


unsigned int IotDevice::getSizeOfDag(Dag* dag)
{
	unsigned int currentSize = 0;

	for (list<Tangle*>::iterator it = dag->Tangles.begin(); it != dag->Tangles.end(); ++it)
	{
		currentSize += sizeof((*it));
	}
	return currentSize;
}

bool IotDevice::flushToUpperLayer()
{
	//Push All Data to higher level by requesting a connection
	Dag* flushDag = new Dag();

	list<Tangle*> tangles = list<Tangle*>();

	for (list<Tangle*>::iterator it = this->InternalDag->Tangles.begin(); it != this->InternalDag->Tangles.end(); ++it)
	{
		if ((*it)->confirmationLevel == 0)
		{
			//Add unconfirmed transactions to a new variable to send from network
			flushDag->Tangles.push_back((*it));
		}
	}

	for (list<Tangle*>::iterator it = flushDag->Tangles.begin(); it != flushDag->Tangles.end(); ++it)
	{
		//Remove all unconfirmed transactions from current DAG
		this->InternalDag->Tangles.remove((*it));
	}


	//Flush operation happenes here ......

	return true;
}