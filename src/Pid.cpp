#include <iostream>
#include <Pid.h>


//====================================================
// Simple PID Controller
//====================================================

Pid::Pid(float p, float i, float d)
    : kp(p), ki(i), kd(d), previousError(0.0f), integral(0.0f)
{
}

float Pid::compute(float setpoint, float measurement)
{
    float error = setpoint - measurement;

    integral += error;
    float derivative = error - previousError;
    previousError = error;

    return (kp * error) +
            (ki * integral) +
            (kd * derivative);
}