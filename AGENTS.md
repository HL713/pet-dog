# Repository Guidelines

## Project Structure & Module Organization
This is a Keil MDK-ARM STM32F103 project generated from CubeMX.

- `Core/Inc` and `Core/Src`: application code, startup, clock, and GPIO setup.
- `Oled/`: OLED driver and font/image data used by the display layer.
- `Drivers/`: STM32 HAL and CMSIS vendor libraries; treat as third-party code.
- `MDK-ARM/`: Keil project files and build outputs such as `.axf`, `.hex`, `.map`.
- `Pet.ioc`: CubeMX configuration source of truth for pins, clocks, and peripherals.

## Build, Test, and Development Commands
Use Keil uVision 5 for the normal workflow.

- Open `MDK-ARM/Pet.uvprojx` and run **Build** or **Rebuild**.
- Flash and debug from uVision after a successful build.
- Generated firmware is written to `MDK-ARM/Pet/Pet.hex` and `MDK-ARM/Pet/Pet.axf`.

If you regenerate code in CubeMX, compare the `.ioc` and keep custom code inside `/* USER CODE BEGIN */` blocks.

## Coding Style & Naming Conventions
- Use 4-space indentation and keep line length reasonable.
- Follow STM32/HAL naming patterns: `MX_GPIO_Init`, `OLED_Init`, `OLED_UpdateArea`.
- Use `PascalCase` for functions and types, `UPPER_SNAKE_CASE` for macros, and `camelCase` only if matching existing vendor code is not practical.
- Keep hardware-specific pin logic in the driver layer, not in `main.c`.

## Testing Guidelines
There is no automated test framework in this repository.

- Validate changes on target hardware after every driver or clock change.
- For OLED work, verify initialization, text rendering, and full-screen refresh.
- For clock or GPIO changes, confirm the board boots and the display still responds.

## Commit & Pull Request Guidelines
This checkout does not include git history, so use concise, imperative commits:

- Example: `oled: fix cursor update bounds`
- Example: `core: preserve user code blocks after regeneration`

Pull requests should include:
- A short summary of the change and affected files.
- Notes on hardware tested, especially board type and OLED module.
- Screenshots or photos if the display output changes.

## Configuration Tips
- PB8/PB9 are configured as open-drain outputs; ensure external pull-ups are present on the I2C lines.
- `Pet.ioc` uses HSE + PLL x9 for 72 MHz, so verify the board crystal matches the configuration.

