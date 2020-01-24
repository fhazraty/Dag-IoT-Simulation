#include <iostream>
#include <random>
#include <list>
#include "IotDevice.h"
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>

using namespace std;

std::mutex g_num_mutex;

void sendTransaction(int i, int j, list<IotDevice*>::iterator it)
{
    if ((*it)->AddTransactionToDag(j))
    {
        //cout << "Send To Device " << i << " Transaction " << j << " Transaction stored locally " << endl;
    }
    else {
        //cout << "Send To Device " << i << " Transaction " << j << " Transaction stored locally with a flush " << endl;
    }
}

int main()
{
    ofstream resFile;
    resFile.open("C:\\Results\\results.txt");

    cout << "Mean,deviceCount,StorageSize,MaxTransaction,AllTransactionCount,AllFlushCount" << endl;
    resFile << "Mean,deviceCount,StorageSize,MaxTransaction,AllTransactionCount,AllFlushCount" << endl;
    for (float mean = 0.1; mean < 10; mean += 0.1)
    {
        for (int storageSize = 100; storageSize < 1001; storageSize += 100)
        {
            for (int nTransaction = 100; nTransaction < 1001; nTransaction += 100)
            {
                Tangle* GenesisTangle = new Tangle();
                GenesisTangle->PayloadData = 0;
                GenesisTangle->ParentLeft = NULL;
                GenesisTangle->ParentRight = NULL;
                GenesisTangle->confirmationLevel = 2;

                Tangle* LeftGenesisTangle = new Tangle();
                LeftGenesisTangle->PayloadData = 0;
                LeftGenesisTangle->ParentLeft = GenesisTangle;
                LeftGenesisTangle->ParentRight = GenesisTangle;
                LeftGenesisTangle->confirmationLevel = 2;

                Tangle* RightGenesisTangle = new Tangle();
                RightGenesisTangle->PayloadData = 0;
                RightGenesisTangle->ParentLeft = GenesisTangle;
                RightGenesisTangle->ParentRight = GenesisTangle;
                RightGenesisTangle->confirmationLevel = 2;

                list<Tangle*> latestFullyConfirmedTransactions = list<Tangle*>();
                latestFullyConfirmedTransactions.push_back(LeftGenesisTangle);
                latestFullyConfirmedTransactions.push_back(RightGenesisTangle);

                const int deviceCount = 10; // number of IoT devices

                list<IotDevice*> iotDevices = list<IotDevice*>();
                for (int i = 0; i < deviceCount; i++)
                {
                    IotDevice* ioTDevice = new IotDevice(storageSize, latestFullyConfirmedTransactions, i);
                    iotDevices.push_back(ioTDevice);
                }

                /*for (list<IotDevice*>::iterator it = iotDevices.begin(); it != iotDevices.end(); ++it)
                {
                    cout << "deviceId [" << (*it)->DeviceId << "] size : " << (*it)->GetCurrentSize() << " submitted transactions:" << (*it)->TransactionCounter << " flush :" << (*it)->FlushCounter << endl;
                }*/

                //cout << "started Running!!!!" << endl;
                //cout << "============================================" << std::endl;

                // poisson_distribution
                const int nrolls = 10000; // number of experiments
                ///const int nTransaction = 100;   // maximum number of transactions to distribute

                default_random_engine generator;
                poisson_distribution<int> distribution(mean);

                int p[deviceCount] = {};

                for (int i = 0; i < nrolls; ++i) {
                    int number = distribution(generator);
                    if (number < deviceCount) ++p[number];
                }


                int numberOfTransactions[deviceCount] = {};
                //cout << "poisson_distribution (mean=4.1):" << std::endl;
                for (int i = 0; i < deviceCount; ++i) {
                    numberOfTransactions[i] = p[i] * nTransaction / nrolls;
                    //cout << numberOfTransactions[i] << endl;
                }
                //cout << "============================================" << std::endl;

                list<thread*> threadsForTransactionSubmission = list<thread*>();

                int deviceCounter = 0;
                for (list<IotDevice*>::iterator it = iotDevices.begin(); it != iotDevices.end(); ++it)
                {
                    for (int j = 0; j < numberOfTransactions[deviceCounter]; j++)
                    {
                        thread* t1 = new thread(sendTransaction, deviceCounter, j, it);
                        threadsForTransactionSubmission.push_back(t1);
                    }
                    deviceCounter++;
                }

                for (list<thread*>::iterator it = threadsForTransactionSubmission.begin(); it != threadsForTransactionSubmission.end(); ++it)
                {
                    (*it)->join();
                }

                //cout << "ended Running!!!!" << endl;

                unsigned int allTransactionCount = 0;
                unsigned int allFlushCount = 0;

                for (list<IotDevice*>::iterator it = iotDevices.begin(); it != iotDevices.end(); ++it)
                {
                    allTransactionCount += (*it)->TransactionCounter;
                    allFlushCount += (*it)->FlushCounter;
                    //cout << "deviceId [" << (*it)->DeviceId << "] size : " << (*it)->GetCurrentSize() << " submitted transactions:" << (*it)->TransactionCounter << " flush :" << (*it)->FlushCounter << endl;
                }

                cout << mean << "," << deviceCount << "," << storageSize << "," << nTransaction << "," << allTransactionCount << "," << allFlushCount << endl;
                resFile << mean << "," << deviceCount << "," << storageSize << "," << nTransaction << "," << allTransactionCount << "," << allFlushCount << endl;
            }
        }
    }

    resFile.close();
    return 0;
}