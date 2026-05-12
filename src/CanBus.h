#pragma once

#include <queue>
#include <mutex>
#include <atomic>


#define COOLANT_TEMP 0x101
#define PUMP_PWM 0x102
#define FAN_PWM 0x103

//====================================================
// Simulated CAN Message Structure
//====================================================
typedef struct
{
    int id;
    float data;
} CANMessage;


//====================================================
// CANBUS Class
//====================================================

class CanBus
{
public:

    //------------------------------------------------
    // Constructor
    //------------------------------------------------

    CanBus(std::atomic<bool>& runningFlag);

    //------------------------------------------------
    // Send CAN Message
    //------------------------------------------------

    void sendCANMessage(int id, float data);

    //------------------------------------------------
    // CAN Communication Thread
    //------------------------------------------------

    void canCommunicationTask();

private:

    //------------------------------------------------
    // CAN Message Queue
    //------------------------------------------------

    std::queue<CANMessage> canQueue;

    //------------------------------------------------
    // Mutex Protection
    //------------------------------------------------

    std::mutex canMutex;

    //------------------------------------------------
    // Shared System Running Flag
    //------------------------------------------------

    std::atomic<bool>& systemRunning;
};