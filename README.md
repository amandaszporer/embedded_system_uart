# Embedded Systems UART - MSP430G2553

A menu-driven embedded application for the **TI MSP430G2553** microcontroller, controlled via UART from a PC. The system integrates an LCD display, buzzer, potentiometer (ADC), push button, and serial communication into a single interactive program.

## Features

| # | Command | Description |
|---|---------|-------------|
| 1 | Count up on LCD | Increments a counter on the 16x2 LCD at a configurable delay rate |
| 2 | Circular tone series | Plays a looping sequence of buzzer tones (1000 - 2500 Hz) using PWM |
| 3 | Set delay time | Receives a delay value in milliseconds from the PC via UART |
| 4 | Potentiometer voltage | Reads the potentiometer via ADC10 and displays the voltage on the LCD |
| 5 | "I love my Negev" | Pressing PB1 transmits the string over UART to the PC |
| 6 | Clear LCD | Clears the LCD display and resets all counters |
| 7 | Show menu | Prints the menu on the PC terminal |
| 8 | Sleep | Enters LPM0 low-power mode, wakes on UART receive |
| 9 | Print string to LCD | Receives a string from the PC (up to 32 chars) and displays it on the LCD |

## Hardware Setup

- **MCU:** TI MSP430G2553 (LaunchPad)
- **LCD:** 16x2 character LCD in 4-bit mode
  - Data: P1.4 - P1.7
  - RS: P2.6 | EN: P2.5 | RW: P2.7
- **Buzzer:** P2.4 (Timer1_A2 PWM output)
- **Potentiometer:** P1.3 (ADC10 channel A3, Vref = 3.41 V)
- **Push Button (PB1):** P1.0 (interrupt-driven, active low)
- **UART:** P1.1 (RX) / P1.2 (TX), 9600 baud, 8N1

## Software Architecture

The firmware follows a layered architecture:

```
main.c          FSM loop - dispatches states
  +-- api.c     Application logic for each feature
  +-- halGPIO.c Hardware Abstraction Layer - LCD driver, ISRs, delays, utilities
  +-- bsp.c     Board Support Package - GPIO, ADC, Timer, and UART initialization
```

**Headers** are located in the `header/` directory:

| File | Purpose |
|------|---------|
| `app.h` | FSM state and system mode enumerations |
| `api.h` | API function prototypes |
| `halGPIO.h` | HAL prototypes, LCD macros, extern declarations |
| `bsp.h` | BSP prototypes, pin/port abstractions, peripheral defines |

### State Machine

The main loop runs a finite state machine (FSM). States are selected by the character received over UART (`'1'` - `'9'`). The MCU enters LPM0 while idle and wakes on UART RX interrupts.

### Interrupts

| Vector | Purpose |
|--------|---------|
| `PORT1_VECTOR` | PB1 press - sends "I love my Negev" over UART |
| `TIMER0_A0_VECTOR` | Timer A0 - used for configurable delays |
| `ADC10_VECTOR` | ADC conversion complete |
| `USCIAB0TX_VECTOR` | UART TX - sends status/command bytes to PC |
| `USCIAB0RX_VECTOR` | UART RX - receives commands and data from PC |

## PC Interface (Python)

`test.py` is a serial terminal application that communicates with the MSP430 over UART.

### Requirements

- Python 3
- `pyserial` (`pip install pyserial`)

### Usage

1. Update `SERIAL_PORT` in `test.py` to match your system (e.g., `COM5` on Windows, `/dev/ttyACM0` on Linux).
2. Run:
   ```
   python test.py
   ```
3. Select options 1-9 from the displayed menu.

## Building the Firmware

This project is intended to be built with **TI Code Composer Studio (CCS)** or **IAR Embedded Workbench** targeting the MSP430G2553. Import the source files into your IDE project and build/flash to the LaunchPad.
