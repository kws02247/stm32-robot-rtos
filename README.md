# STM32 Bare-Metal Mini-RTOS

A lightweight, bare-metal Real-Time Operating System (RTOS) developed in C and ARM Assembly for the STM32 NUCLEO-F446RE microcontroller.

This project focuses on understanding and implementing fundamental RTOS mechanisms from scratch, including task management, context switching, interrupt handling, and scheduling, without relying on an existing RTOS such as FreeRTOS.

The RTOS is designed to support real-time applications, including a two-wheel self-balancing robot.

## Key Features

- **Bare-Metal Firmware:** Custom startup code, interrupt vector table, and linker script.
- **Task Management:** Static task creation, dedicated task stacks, and Task Control Blocks (TCBs).
- **Context Switching:** ARM Assembly-based register saving and restoring using PendSV.
- **Task Scheduling:** Round-robin scheduler for switching between multiple tasks.
- **System Timer:** SysTick-based 1 ms system tick for task timing.
- **Task Delays:** Tick-based task delays and task state management.
- **Peripheral Drivers:** Low-level UART, GPIO, and I2C communication.

## System Architecture

The firmware is organized into separate components:

```text
stm32-robot-rtos/
├── app/           # Application and robot tasks
├── drivers/       # Hardware peripheral drivers
├── kernel/        # RTOS kernel and scheduler
├── startup/       # ARM startup and exception handlers
├── linker.ld      # Flash and RAM memory layout
└── Makefile       # Build configuration
```

The RTOS uses the ARM Cortex-M exception mechanism to handle task switching:

1. **SysTick** provides periodic system timer interrupts.
2. **Scheduler** selects the next ready task.
3. **PendSV** performs context switching between tasks.
4. **PSP/MSP** separates task stack execution from exception handling.

## Hardware & Development Tools

| Component | Description |
|---|---|
| Microcontroller | STM32F446RE (ARM Cortex-M4) |
| Development Board | NUCLEO-F446RE |
| Languages | C, ARM Assembly |
| Compiler | ARM GNU Toolchain (`arm-none-eabi-gcc`) |
| Debugging / Flashing | ST-Link, st-flash |
| Communication | UART, I2C |
| Build System | Make |

## Build & Flash

The project uses a Makefile and the ARM GNU Toolchain.

```bash
# Build firmware
make
```

Firmware flashing can be performed using ST-Link and the generated firmware image.

Refer to the Makefile for the configured build targets and output filenames.

## Application: Self-Balancing Robot

The Mini-RTOS also provides the foundation for a two-wheel self-balancing robot.

The robot firmware separates real-time operations into independent tasks:

- IMU sensor acquisition (MPU6050)
- Angle estimation using a complementary filter
- PID-based balance control
- Motor communication and control
- Safety monitoring
- UART-based debugging and logging

The integrated robot project successfully achieved two-wheel balancing using sensor feedback and PID control.

## Project Goals

This project demonstrates practical experience in:

- Bare-metal embedded software development
- ARM Cortex-M architecture and exception handling
- RTOS kernel design and task scheduling
- Low-level hardware interfacing
- Real-time robotic control

## Technical Walkthrough

A video series explaining the RTOS implementation, architecture, and context-switching mechanisms is linked here.
<https://www.youtube.com/playlist?list=PLZ7AIEb2Qmww>

*Developed by Wooseok(Kyle) Kim
