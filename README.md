# pet-dog

STM32F103C8T6-based quadruped robot dog with voice control (ASRPRO), OLED display, and modular motion/state system.

## Current architecture

- `Core/Src/asr.c`: UART input and command parsing for ASRPRO.
- `Core/Src/dog_state.c`: command-to-state bridge.
- `Core/Src/dog_motion.c`: pose sequencing, servo framing, and OLED face switching.
- `Core/Src/servo.c`: PWM output and calibration for five servos.
- `Core/Src/main.c`: startup flow and task scheduling.

## Notes

- The project is generated from CubeMX and built with Keil MDK-ARM.
- Keep custom logic inside `/* USER CODE BEGIN */` blocks when regenerating.
- Build outputs under `MDK-ARM/` should stay untracked.
- This repository is the source repository only.
- Local reference material such as `开源版本/`, `解释/`, and `pcb原理图/` is kept outside version control and excluded by `.gitignore`.
