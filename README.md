# STM32 Data Acquisition and Storage System

## Project Overview

This project is a beginner-level STM32 embedded systems project based on the STM32F103 microcontroller. It is designed to practice commonly used STM32 peripherals, data acquisition, data storage, and serial command interaction.

The system uses TIM3 to periodically trigger ADC1 for data acquisition and DMA to automatically transfer the sampled data to a memory buffer. After a group of samples is collected, the data is processed and stored in an external W25Q64 Flash memory.

The system also provides a USART1-based serial command interface. A Ring Buffer is used to receive serial data, and a simple command parser is implemented for historical data queries and LED control.

The current version is implemented using a bare-metal architecture and focuses on the integration of multiple STM32 peripherals and basic embedded software design.

## Features

- Periodic ADC sampling triggered by TIM3
- ADC1 + DMA for automatic data transfer
- 64 ADC samples per data group
- Average value calculation for each data group
- W25Q64 external Flash data storage
- USART1 serial communication
- Ring Buffer for serial data reception
- Serial command parsing
- OLED status display
- TIM2 PWM-based LED brightness control
- Button-based LED control

## Serial Commands

The system supports the following commands:

```text
HELP
LED ON
LED OFF
LED State
History Index Show
History data <Group> <Index>
```

For example:

```text
History data 3 10
```

This command reads the 10th sample from the 3rd stored data group.

Other commands:

- `HELP` - Display the list of supported commands.
- `LED ON` - Turn on the LED.
- `LED OFF` - Turn off the LED.
- `LED State` - Display the current LED state.
- `History Index Show` - Display the number of stored data groups.
- `History data <Group> <Index>` - Read a specific sample from a stored data group.

## Hardware Platform

- MCU: STM32F103
- External Flash: W25Q64
- Display: OLED
- Communication: USART1
- ADC: ADC1
- DMA: DMA1 Channel 1
- PWM: TIM2
- ADC Trigger: TIM3

## Software Environment

- Keil MDK
- STM32F10x Standard Peripheral Library
- ARM Compiler

## Project Structure

```text
STM32-Data-Acquisition-System/
├── Hardware/              # Hardware drivers and application modules
├── Library/               # STM32F10x Standard Peripheral Library
├── Start/                 # CMSIS and startup files
├── User/                  # Main program and interrupt handlers
├── Docs/                  # Project photos and screenshots
├── .gitignore
├── Project.uvprojx        # Keil project file
└── README.md
```

## Project Highlights

This project integrates several commonly used STM32 peripherals and embedded software techniques, including ADC, DMA, timers, USART, Ring Buffer, SPI Flash, OLED, and PWM.

The current version uses a bare-metal architecture. The main loop handles data processing, serial command parsing, and button detection, while interrupts and DMA are used for real-time data transfer.

Compared with individual peripheral experiments, this project focuses more on the cooperation between multiple functional modules and uses a modular structure to organize the driver and application code.

## Demo

### Hardware Setup

The following photo shows the hardware setup used for this project.

![Hardware Setup](Docs/hardware_setup.jpg)

### Serial Terminal

The serial terminal can be used to send commands and query stored data.

![Serial Terminal](Docs/serial_terminal.png)

