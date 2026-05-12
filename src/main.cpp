#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <queue>
#include <chrono>
#include "Pid.h"
#include "CanBus.h"



using namespace std;

//====================================================
// Cooling System States
//====================================================

enum class SystemState
{
    INIT,
    IDLE,
    RUNNING,
    COOLING,
    FAULT
};

//====================================================
// Shared Variables
//====================================================
atomic<bool> systemRunning(true);
mutex dataMutex;
SystemState currentState = SystemState::INIT;

//====================================================
// Inputs
//====================================================

bool ignitionSwitch = true;
bool coolantLevelLow = false;
bool pumpFeedback = true;
bool emergencyStop = false;

float coolantTempC = 25.0f;

//====================================================
// Outputs
//====================================================

bool pumpEnable = false;
bool fanEnable = false;
bool derateRequest = false;
bool faultActive = false;

//====================================================
// PWM Output Simulation
//====================================================

int pumpPWM = 0;
int fanPWM = 0;

//====================================================
// Temperature Thresholds
//====================================================
const float FAN_ON_TEMP = 50.0f;
const float FAN_OFF_TEMP = 45.0f;
const float CRITICAL_TEMP = 70.0f;

float targetTemperature = FAN_OFF_TEMP;


//====================================================
// Simulated Temperature Profile
//====================================================

vector<float> temperatureProfile =
{
    30.0f,
    35.0f,
    40.0f,
    45.0f,
    50.0f,
    55.0f,
    60.0f,
    65.0f,
    72.0f,
    68.0f,
    55.0f,
    45.0f,
    38.0f
};


//====================================================
// Cooling Controller Thread
//====================================================
void coolingControllerTask(CanBus& canBus)
{
    Pid pumpPID(2.0f, 0.1f, 0.05f);

    while(systemRunning)
    {
        {
            lock_guard<mutex> lock(dataMutex);

            //------------------------------------------------
            // Safety Checks
            //------------------------------------------------

            if(coolantLevelLow || !pumpFeedback || emergencyStop)
            {
                faultActive = true;
                currentState = SystemState::FAULT;
            }

            if(coolantTempC >= CRITICAL_TEMP)
            {
                faultActive = true;
                derateRequest = true;
                currentState = SystemState::FAULT;
            }

            //------------------------------------------------
            // State Machine
            //------------------------------------------------

            switch(currentState)
            {
                case SystemState::INIT:
                {
                    cout << "[STATE] INIT" << endl;

                    currentState = SystemState::IDLE;
                    break;
                }

                case SystemState::IDLE:
                {
                    cout << "[STATE] IDLE" << endl;

                    pumpEnable = false;
                    fanEnable = false;

                    if(ignitionSwitch)
                    {
                        currentState = SystemState::RUNNING;
                    }

                    break;
                }
                case SystemState::RUNNING:
                {
                    cout << "[STATE] RUNNING" << endl;

                    pumpEnable = true;

                    //------------------------------------------------
                    // PID-Based Pump Speed Control
                    //------------------------------------------------
                    float pidOutput = pumpPID.compute(targetTemperature,
                                                      coolantTempC);

                    pumpPWM = static_cast<int>(100 - pidOutput);

                    if(pumpPWM < 0)
                    {
                        pumpPWM = 0;
                    }

                    if(pumpPWM > 100)
                    {
                        pumpPWM = 100;
                    }

                    //------------------------------------------------
                    // Fan Activation
                    //------------------------------------------------

                    if(coolantTempC >= FAN_ON_TEMP)
                    {
                        currentState = SystemState::COOLING;
                    }

                    break;
                }

                case SystemState::COOLING:
                {
                    cout << "[STATE] COOLING" << endl;

                    pumpEnable = true;
                    fanEnable = true;
                    fanPWM = 100;

                    if(coolantTempC <= targetTemperature)
                    {
                        fanEnable = false;
                        fanPWM = 0;

                        currentState = SystemState::RUNNING;
                    }

                    break;
                }

                case SystemState::FAULT:
                {
                    cout << "[STATE] FAULT" << endl;

                    pumpEnable = false;
                    fanEnable = true;

                    pumpPWM = 0;
                    fanPWM = 100;

                    derateRequest = true;

                    break;
                }
            }
            //------------------------------------------------
            // CANBUS Message Transmission
            //------------------------------------------------
            canBus.sendCANMessage(COOLANT_TEMP, coolantTempC);
            canBus.sendCANMessage(PUMP_PWM, pumpPWM);
            canBus.sendCANMessage(FAN_PWM, fanPWM);

            //------------------------------------------------
            // System Status Output
            //------------------------------------------------

            cout << "Temperature : "
                 << coolantTempC
                 << " C"
                 << endl;

            cout << "Pump        : "
                 << (pumpEnable ? "ON" : "OFF")
                 << endl;

            cout << "Fan         : "
                 << (fanEnable ? "ON" : "OFF")
                 << endl;

            cout << "Pump PWM    : "
                 << pumpPWM
                 << "%"
                 << endl;

            cout << "Fan PWM     : "
                 << fanPWM
                 << "%"
                 << endl;

            cout << "Fault       : "
                 << (faultActive ? "YES" : "NO")
                 << endl;

            cout << "Derate      : "
                 << (derateRequest ? "YES" : "NO")
                 << endl;

            cout << "--------------------------------------"
                 << endl;
        }

        //------------------------------------------------
        // Controller Execution Rate
        //------------------------------------------------
        this_thread::sleep_for(chrono::seconds(1));
    }
}


//====================================================
// Temperature Sensor Simulation Thread
//====================================================
void temperatureSimulationTask()
{
    for(float temp : temperatureProfile)
    {
        {
            lock_guard<mutex> lock(dataMutex);

            coolantTempC = temp;

            // For debug purpose to see Temeperature changes
            // cout << "[Temperature Sensor] "
            //      << coolantTempC
            //      << " C"
            //      << endl;
        }

        this_thread::sleep_for(chrono::seconds(2));
    }

    //------------------------------------------------
    // Stop System After Simulation
    //------------------------------------------------

    systemRunning = false;
}


//====================================================
// Main Function
//====================================================

int main(int argc, char* argv[])
{
    cout << "====================================" << endl;
    cout << "Cooling Firmware Simulation" << endl;
    cout << "====================================" << endl;

    // User-defined setpoint
    // User MUST send a correct set-point(float format) 
    // only 1 argument is accepted, rest will be ignored
    if(argc > 1)
    {
        targetTemperature = stof(argv[1]);
    }

    cout << "Target Temperature = "
         << targetTemperature
         << " C"
         << endl;

    //------------------------------------------------
    // Create Threads
    //------------------------------------------------
    CanBus canBus(systemRunning);
    // 
    thread controllerThread(coolingControllerTask, std::ref(canBus));
    thread temperatureThread(temperatureSimulationTask);
    thread canThread(&CanBus::canCommunicationTask, &canBus);

    //------------------------------------------------
    // Wait For Thread Completion
    //------------------------------------------------
    temperatureThread.join();
    controllerThread.join();
    canThread.join();

    cout << endl;
    cout << "Firmware Shutdown Complete" << endl;

    return 0;
}