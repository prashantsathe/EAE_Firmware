#pragma once

class Pid
{
private:
    float kp;
    float ki;
    float kd;

    float previousError;
    float integral;

public:
    Pid(float p, float i, float d);
    float compute(float setpoint, float measurement);
};
