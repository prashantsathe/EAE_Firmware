\# EAE Cooling System Firmware



This repository contains the C++ firmware implementation for the \*\*EAE Electrical and Controls Challenge\*\*. The system manages a high-performance vehicle cooling loop



\## System Architecture

The firmware is built using a multithreaded approach to simulate real-world embedded control environments.



\- \*\*State Machine\*\*: Orchestrates transitions between `INIT`, `IDLE`, `RUNNING`, `COOLING`, and `FAULT`.

\- \*\*PID Control\*\*: Implements a  algorithm for precise pump and fan regulation.

\- \*\*Asynchronous CANBUS\*\*: A dedicated thread-safe queue simulates 

\- \*\*Safety Interlocks\*\*: Implements critical shutdown logic based on the temperature thresholds.



\---



\## Project Structure



```text

EAE\_Firmware/

├── CMakeLists.txt        # Build system configuration

├── launch.sh             # Automation script for MSYS2

├── README.md             # Project documentation

├── src/

│   ├── main.cpp          # System orchestration and entry point

│   ├── Pid.cpp / .h      # PID Control logic

│   └── CanBus.cpp / .h   # Thread-safe CAN simulation

└── tests/

&#x20;   └── test\_main.cpp     # GTest unit tests

```



\## Prerequisites

This project is designed for the MSYS2 MinGW 64-bit environment. Ensure the following are installed:

```

pacman -S --needed mingw-w64-x86\_64-cmake mingw-w64-x86\_64-toolchain

```



\## How to Build and Run

The project includes a launch.sh script that handles CMake configuration, compilation, and execution in one step. 

It allows you to pass a custom temperature setpoint as a runtime argument.

Finally it runs the Gtest test cases 



```

$ ./launch.sh

or

$ ./launch.sh 70   ##  Custom Setpoint Launch (e.g., 70°C):

```



