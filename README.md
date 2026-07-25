# ARM-STM32F446RE-SIM-Driver-Core

A comprehensive, **bare-metal** (register-level, no ST HAL/LL) driver suite for the **STM32F446RE** ARM Cortex-M4 MCU. This is an "all-protocols driver" architecture — one consistent driver framework covering **USART, I2C, SPI, CAN, USB OTG, SAI, and SDIO** — built for high-reliability embedded cellular applications (modem/SIM control, telemetry, local storage, and audio I/O on a single MCU).

> **Status:** actively under construction. See [Implementation Status](#implementation-status) — the CMSIS/register foundation and driver framework are in place; protocol drivers are being added incrementally.

---

## Why bare-metal

- **Direct register access** against the official CMSIS device header — no HAL/LL abstraction layers or their runtime overhead.
- **Deterministic timing and small footprint**, which matters for cellular firmware that has to service a modem UART, a CAN bus, and storage I/O without jitter.
- **Full control of the clock tree, DMA, and interrupt priorities** instead of inheriting CubeMX defaults.
- **Portable core**: the driver layer depends only on CMSIS register definitions, so it isn't tied to STM32CubeIDE — it builds with a plain `arm-none-eabi-gcc` + Makefile toolchain too.

## Target hardware

| Item | Detail |
|---|---|
| MCU | STM32F446RETx (LQFP64) |
| Core | ARM Cortex-M4F, up to 180 MHz, single-precision FPU |
| Flash | 512 KB |
| SRAM | 128 KB |
| Reference board | Nucleo-F446RE (bring-up), custom cellular carrier board (target) |
| Peripherals driven | USART1/2/3/6 · I2C1/2/3 · SPI1/2/3/4 · CAN1/CAN2 (bxCAN) · USB OTG FS · SAI1 · SDIO |

## Architecture

```
 Application  (Examples/, Core/Src/main.c)
        │
 Drivers/DriverCore     ← this repo: USART / I2C / SPI / CAN / SAI / SDIO / USB OTG
        │                  drivers, driver_core.h common status codes + GPIO/clock helpers
 Drivers/CMSIS           ← unmodified, official ST CMSIS device headers (register defs)
        │                  + ARM CMSIS-Core (core_cm4.h, intrinsics)
      Silicon             STM32F446RE
```

Each protocol driver is self-contained (its own `*_driver.h` / `*_driver.c` pair), returns a shared `drv_status_t` result code, and only touches its own peripheral's registers plus the common GPIO/clock helpers in `driver_core.h` — so drivers can be dropped into other STM32F4 projects independently.

## Repository structure

```
ARM-STM32F446RE-SIM-Driver-Core/
├── README.md
├── STM32F446RETX_FLASH.ld          # linker script (512K flash / 128K RAM map)
├── Core/
│   ├── Inc/                        # main.h, stm32f4xx_it.h (interrupt vector handlers)
│   ├── Src/                        # main.c, stm32f4xx_it.c, system_stm32f4xx.c
│   └── Startup/
│       └── startup_stm32f446xx.s   # reset handler + vector table (official ST template)
├── Drivers/
│   ├── CMSIS/
│   │   ├── Include/                # core_cm4.h, cmsis_gcc.h, cmsis_compiler.h ...
│   │   └── Device/ST/STM32F4xx/
│   │       ├── Include/            # stm32f446xx.h, stm32f4xx.h  (register defs)
│   │       └── Source/             # system_stm32f4xx.c (clock/vector table init)
│   └── DriverCore/                 # ★ the driver suite itself
│       ├── Inc/
│       │   ├── driver_core.h       # common status codes, GPIO helpers, tick timer
│       │   ├── usart_driver.h
│       │   ├── i2c_driver.h
│       │   ├── spi_driver.h
│       │   ├── can_driver.h
│       │   ├── sdio_driver.h
│       │   ├── sai_driver.h
│       │   └── usb_otg_driver.h
│       └── Src/                    # matching *_driver.c implementations
├── Examples/
│   └── Src/                        # cellular/AT-command, SD logging, CAN telemetry demos
└── docs/                           # peripheral notes / register maps
```

## Implementation status

| Layer / Peripheral | File(s) | Status |
|---|---|---|
| CMSIS register headers + startup + linker script | `Drivers/CMSIS/*`, `Core/Startup/*`, `*.ld` | ✅ Complete (sourced from ST's official CMSIS packs) |
| Common driver framework (status codes, GPIO/clock/tick helpers) | `driver_core.h` / `.c` | ✅ Complete |
| USART (polling + interrupt, AT-command helper) | `usart_driver.*` | 🚧 In progress |
| I2C (master, polling) | `i2c_driver.*` | ⏳ Planned |
| SPI (polling/DMA) | `spi_driver.*` | ⏳ Planned |
| CAN — bxCAN | `can_driver.*` | ⏳ Planned |
| SDIO — SD card init + block R/W | `sdio_driver.*` | ⏳ Planned |
| SAI — I2S-style audio transfer | `sai_driver.*` | ⏳ Planned |
| USB OTG FS device (CDC-ACM virtual COM) | `usb_otg_driver.*` | ⏳ Planned |
| `main.c` / example applications | `Core/Src/main.c`, `Examples/` | ⏳ Planned |
| Build system | Makefile (verified with `arm-none-eabi-gcc`) + STM32CubeIDE project | ⏳ Planned |

Legend: ✅ complete · 🚧 in progress · ⏳ not started yet.

## Getting started

**Toolchain target: STM32CubeIDE.**

1. Clone/copy this repository.
2. In STM32CubeIDE: `File → New → STM32 Project` targeting **STM32F446RETx**, or `File → Import → Existing Code as Makefile Project` once the Makefile lands.
3. Add to the project's include paths:
   - `Drivers/CMSIS/Include`
   - `Drivers/CMSIS/Device/ST/STM32F4xx/Include`
   - `Drivers/DriverCore/Inc`
   - `Core/Inc`
4. Add all `.c` files under `Core/Src`, `Drivers/CMSIS/Device/ST/STM32F4xx/Source`, and `Drivers/DriverCore/Src` to the build.
5. Use `Core/Startup/startup_stm32f446xx.s` as the startup file and `STM32F446RETX_FLASH.ld` as the linker script.
6. Build and flash via ST-Link.

A CLI-only path (`arm-none-eabi-gcc` + Makefile) is also being added so the driver suite can be smoke-compiled outside the IDE; each driver in this repo is being validated against the official CMSIS register headers as it's written.

## License

No license — all rights reserved. This is currently a private/proprietary codebase; there is no open redistribution or contribution license attached.

## Roadmap

- [ ] USART driver + AT-command convenience layer for cellular modem control
- [ ] I2C, SPI master drivers
- [ ] bxCAN driver (CAN1/CAN2)
- [ ] SDIO driver with SD card block I/O
- [ ] SAI audio driver
- [ ] USB OTG FS device stack (CDC-ACM virtual COM)
- [ ] Example applications tying protocols together (modem AT console, SD logger, CAN telemetry)
- [ ] Makefile build verified end-to-end with `arm-none-eabi-gcc`
- [ ] Native STM32CubeIDE `.project` / `.cproject` metadata
