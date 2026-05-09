# AgniSense_
# QNX SDV + Ambulance V2V System

Real-time Software Defined Vehicle (SDV) and Ambulance Vehicle-to-Vehicle (V2V) communication system developed on QNX Neutrino RTOS using Raspberry Pi 4, HC-05 Bluetooth communication, ultrasonic sensing, hall sensor speed monitoring, real-time threading, mutex synchronization, and native QNX IPC.

---

# Overview

This project demonstrates a real-time intelligent vehicle safety and emergency communication framework capable of operating in two modes:

* **SDV Mode**
* **Ambulance Emergency Mode**

The system continuously monitors obstacle distance and vehicle speed, dynamically controls motor speed, generates alert messages, and communicates wirelessly using HC-05 Bluetooth modules.

The implementation uses:

* QNX Neutrino RTOS
* POSIX real-time threads
* SCHED_FIFO scheduling
* QNX native message passing
* Mutex-protected shared resources
* GPIO memory-mapped access

---

# Features

## Real-Time QNX Features

* Native QNX message passing using `ChannelCreate()` and `MsgSendPulse()`
* Deterministic thread execution using `SCHED_FIFO`
* Priority-based scheduling
* Mutex synchronization for shared data protection
* Multi-threaded architecture
* Real-time event handling

## Vehicle Features

* Automatic switching between:

  * SDV mode
  * Ambulance mode
* Real-time obstacle detection
* Dynamic motor speed control
* Wireless V2V communication
* RPM and speed monitoring
* Scenario visualization LEDs
* Emergency alert indication

---

# Hardware Components

| Component          | Purpose                |
| ------------------ | ---------------------- |
| Raspberry Pi 4     | Main controller        |
| HC-SR04            | Distance measurement   |
| KY-024 Hall Sensor | RPM/speed sensing      |
| HC-05 Bluetooth    | V2V communication      |
| Motor Driver       | Motor control          |
| DC Motors          | Vehicle movement       |
| White LED          | SDV alerts             |
| Red LED            | Emergency alerts       |
| Scenario LEDs      | Scenario visualization |

---

# GPIO Connections

## HC-SR04 Ultrasonic Sensor

| Pin  | GPIO  |
| ---- | ----- |
| TRIG | GPIO5 |
| ECHO | GPIO6 |

## KY-024 Hall Sensor

| Pin | GPIO   |
| --- | ------ |
| D0  | GPIO19 |

## Alert LEDs

| LED       | GPIO   |
| --------- | ------ |
| WHITE LED | GPIO20 |
| RED LED   | GPIO21 |

## Scenario LEDs

| LED  | GPIO   |
| ---- | ------ |
| LED1 | GPIO24 |
| LED2 | GPIO25 |
| LED3 | GPIO12 |
| LED4 | GPIO16 |

## HC-05 Bluetooth

| HC05 Pin | Raspberry Pi GPIO |
| -------- | ----------------- |
| TX       | GPIO15 (RX)       |
| RX       | GPIO14 (TX)       |

## Motor Driver

| Pin | GPIO   |
| --- | ------ |
| IN1 | GPIO17 |
| ENA | GPIO18 |
| IN3 | GPIO22 |
| ENB | GPIO23 |

---

# System Architecture

The application is divided into multiple real-time threads:

| Thread               | Function                          |
| -------------------- | --------------------------------- |
| Supervisor Thread    | Handles QNX pulses and monitoring |
| Sensor Thread        | Ultrasonic distance sensing       |
| Hall Thread          | RPM and speed calculation         |
| Mode Thread          | SDV/Ambulance mode switching      |
| LED Thread           | Alert indication                  |
| Scenario Thread      | Scenario LED sequencing           |
| Communication Thread | HC-05 V2V messaging               |

---

# Real-Time Scheduling

The project uses `SCHED_FIFO` priority scheduling.

| Thread        | Priority |
| ------------- | -------- |
| Supervisor    | 40       |
| Sensor        | 35       |
| Hall Sensor   | 30       |
| Mode          | 25       |
| LED           | 20       |
| Scenario      | 15       |
| Communication | 10       |

---

# V2V Communication Logic

## SDV Mode

| Distance        | Message           |
| --------------- | ----------------- |
| d > 60 cm       | CLEAR             |
| 40 < d <= 60 cm | TRAFFIC           |
| 20 < d <= 40 cm | OBSTACLE          |
| d <= 20 cm      | CRITICAL OBSTACLE |

## Ambulance Mode

| Distance        | Message        |
| --------------- | -------------- |
| d > 60 cm       | ARRIVING       |
| 40 < d <= 60 cm | APPROACHING    |
| 20 < d <= 40 cm | GIVE WAY ALERT |
| d <= 20 cm      | EMERGENCY STOP |

---

# Motor Control Logic

| Distance       | Motor Action  |
| -------------- | ------------- |
| d < 20 cm      | Stop          |
| 20 ≤ d < 60 cm | Reduced Speed |
| d ≥ 60 cm      | High Speed    |

In ambulance mode, maximum speed is enabled during safe distances.

---

# Mutex Protected Shared Resources

The following shared resources are protected using POSIX mutexes:

* Distance data
* RPM/speed data
* Emergency mode state
* Bluetooth messages

Mutexes used:

```c
pthread_mutex_t distance_mutex
pthread_mutex_t rpm_mutex
pthread_mutex_t mode_mutex
pthread_mutex_t message_mutex
```

---

# QNX IPC Mechanism

The system uses native QNX IPC:

* `ChannelCreate()`
* `ConnectAttach()`
* `MsgSendPulse()`
* `MsgReceive()`

Pulse codes:

```c
#define PULSE_ULTRA 1
#define PULSE_HALL  2
```

---

# Speed Calculation

Vehicle speed is calculated using:

* Hall sensor pulse timing
* RPM computation
* Wheel circumference conversion

Wheel diameter used:

```c
0.06 meters
```

---

# Building the Project

## Compile

```bash
qcc final_code.c -o sdv_system -lpthread
```

## Run

```bash
./sdv_system
```

---

# Required QNX Permissions

The application requires I/O privileges:

```c
ThreadCtl(_NTO_TCTL_IO,0);
```

---

# Expected Console Output

```text
MODE:SDV | DIST:75.32 cm | SPEED:3.20 km/hr | MSG:CLEAR

MODE:AMBULANCE | DIST:28.45 cm | SPEED:5.10 km/hr | MSG:GIVE WAY ALERT
```

---

# Key Concepts Demonstrated

* Real-time embedded systems
* QNX RTOS programming
* Thread synchronization
* Priority scheduling
* Inter-process communication
* Memory-mapped GPIO
* Wireless V2V communication
* Automotive safety systems
* Intelligent transportation systems

---

# Applications

* Smart ambulance systems
* Collision avoidance systems
* Autonomous vehicle communication
* Real-time SDV research
* Embedded automotive platforms
* Emergency traffic management

---

# Future Improvements

* Camera-based obstacle detection
* CAN bus integration
* AI-based traffic prediction
* GPS integration
* LTE/5G V2X communication
* Adaptive cruise control
* Real-time dashboard UI
* Sensor fusion

---

# Authors

G V S K Akhil, E Deepsika Reddy, Shaik Sarah
