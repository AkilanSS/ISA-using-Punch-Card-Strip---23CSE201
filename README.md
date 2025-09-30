# Punch-Card Strip Instruction Set Architecture (ISA) on Arduino

## Project Developers
Adhitya Ranjith - CB.SC.U4CSE24703 \
Adwit Singh - CB.SC.U4CSE24704 \
Akilan S S - CB.SC.U4CSE24707 \
Naveen Velan - CB.SC.U4CSE24734

## Project Description

This project documents the development of a custom Instruction Set Architecture (ISA) and a physical reader system using an Arduino. The system reads a series of punched-hole patterns from a continuous strip, interprets these patterns as low-level machine instructions, and executes them. The project explores fundamental concepts in computer architecture, embedded systems, and C programming by recreating a historical computing paradigm.

## Hardware Components

* **Arduino Uno:** The microcontroller that serves as the "CPU" of the system.
* **Stepper Motors (x2):** Two motors with drivers to precisely control the forward and backward movement of the punch-card strip.
* **LDR Sensors (x6):** A sensor array to detect the presence or absence of holes in the punch-card strip.
* **Buzzer (x1):** An audio output device for simple signals or status updates.
* **LED Matrix (x1):** A visual display for outputting data, messages, or simple animations.

## Software Structure

The project's code is organized into a modular structure to promote readability and maintainability.

```/23CSE201 - Punch Card Based Instruction Set Architecture
├── /src
│   ├── /modules
│   │   ├── LogicProcessor
│   │   │   ├── LogicProcessor.h
│   │   │   └── LogicProcessor.cpp
│   │   ├── OutputProcessor
│   │   │   ├── OutputProcessor.h
│   │   │   └── OutputProcessor.cpp
│   │   └── SensorReader
│   │       ├── SensorReader.h
│   │       └── SensorReader.cpp
│   │   └── StepperControl
│   │       ├── StepperControl.h
│   │       └── StepperControl.cpp
│   │
│   └── main.ino
│
├── /lib
│   └──
│
├── /docs
│   └──
│
└── README.md

```

* `main.ino`: The main Arduino sketch file. It contains the `setup()` and `loop()` functions and handles the high-level logic and state transitions of the program.
* `src/modules/`: This directory contains separate, modular C++ files, each responsible for a specific part of the system.
    * `LogicProcessor/`: Manages the decoding and execution of instructions. It contains the core logic for the ISA.
    * `OutputProcessor/`: Handles all output functionality, including controlling the buzzer and the LED matrix display.
    * `SensorReader/`: Manages the input from the LDR sensor array. It reads the raw sensor values and translates them into binary instruction patterns.
    * `StepperControl/`: Controls the stepper motors for strip advancement and navigation (e.g., handling jump instructions).

## Getting Started

1.  Clone this repository to your local machine.
2.  Install the required libraries (e.g., for the stepper motor drivers, LED matrix, etc.) through the Arduino IDE's Library Manager.
3.  Open the `main.ino` file in the Arduino IDE.
4.  Connect the hardware as specified in the project documentation (located in the `docs/` directory).
5.  Upload the code to your Arduino Uno and begin feeding in your punch-card strips.