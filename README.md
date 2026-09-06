# Bare-Metal Firmware Architecture, Interrupt-Driven SPI/DMA & FreeRTOS Task Scheduling
Register-level embedded firmware for ARM Cortex-M (STM32), bypassing HAL/LL 
abstraction layers. Integrates FreeRTOS for deterministic multitasking and 
DMA-backed, interrupt-driven SPI transfers.

## Design
* **Hardware Register Layer:** Direct manipulation of RCC, GPIOD, SPI1, and 
  DMA2 registers — no HAL/LL dependency.
* **Interrupt-Driven DMA Transfers:** SPI1 TX offloaded to DMA2 Stream3; a 
  transfer-complete interrupt hands control to a FreeRTOS task via a binary 
  semaphore (`xSemaphoreGiveFromISR`), rather than CPU polling.
* **Kernel Task Multi-threading:** Three FreeRTOS tasks — sensor telemetry 
  (kicks off DMA transfers), actuator control (GPIO toggling), and a DMA 
  transfer-complete consumer.

## Verification status
This code has been written and reviewed against the STM32F4 reference manual 
(RM0090) register map, but has **not yet been compiled/flashed on hardware** — 
verify the DMA stream/channel mapping and DMA2_Stream3_IRQn number against your 
exact STM32F4 part variant and CMSIS startup file before relying on it. To be 
updated once tested on an STM32F4 Discovery board.

## Roadmap
1. RX-side DMA for full-duplex SPI transfers.
2. NVIC priority grouping tuned against FreeRTOS's configured priority range.
3. Low-power (sleep-on-idle) integration for the actuator control task.
