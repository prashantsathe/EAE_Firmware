#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include "CanBus.h"

using namespace std;

//====================================================
// Constructor
//====================================================
CanBus::CanBus(atomic<bool>& runningFlag)
    : systemRunning(runningFlag) // should be at initialization state
{
    // systemRunning = runningFlags .. is wrong way of doing it, won't be a alias
}

//====================================================
// CAN Transmit Function
//====================================================
void CanBus::sendCANMessage(int id, float data)
{
    lock_guard<mutex> lock(canMutex);

    CANMessage msg;
    msg.id = id;
    msg.data = data;

    canQueue.push(msg);
}

//====================================================
// CAN Communication Thread
//====================================================
void CanBus::canCommunicationTask()
{
    while(systemRunning)
    {
        {
            lock_guard<mutex> lock(canMutex);

            while(!canQueue.empty())
            {
                CANMessage msg = canQueue.front();
                canQueue.pop();

                cout << "[CAN TX] ID: 0x"
                    << hex
                    << msg.id
                    << dec
                    << " Data: "
                    << msg.data
                    << endl;
                cout << "--------------------------------------"
                 << endl;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}