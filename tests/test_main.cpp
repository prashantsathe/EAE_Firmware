#include <gtest/gtest.h>
#include "pid.h"
#include "CanBus.h"
#include <thread>
using namespace std;

// --- PID Controller Tests ---

TEST(PIDTest, PositiveOutput)
{
    Pid pid(2.0f, 0.1f, 0.05f);

    float output = pid.compute(50.0f, 40.0f);

    EXPECT_GT(output, 0.0f);
}

TEST(PIDTest, ZeroError)
{
    Pid pid(2.0f, 0.1f, 0.05f);

    float output = pid.compute(45.0f, 45.0f);

    EXPECT_EQ(output, 0.0f);
}



// --- CanBus Thread-Safety Tests ---

TEST(CanBusTest, MessageQueueing) {
    std::atomic<bool> running(true);
    CanBus canBus(running);
    thread canThread(&CanBus::canCommunicationTask, &canBus);
    
    // Test that we can push messages without crashing (Thread Safety)
    EXPECT_NO_THROW({
        canBus.sendCANMessage(0x101, 55.5f);
        canBus.sendCANMessage(0x102, 100.0f);
    });
    
    // wait for CAN to process the data
    this_thread::sleep_for(chrono::milliseconds(500));
    running = false; // Stop the simulated thread
}