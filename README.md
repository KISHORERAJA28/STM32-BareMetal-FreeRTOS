# Bare-Metal Firmware Architecture & Deterministic FreeRTOS Task Scheduling

This repository hosts a low-level embedded hardware framework written in bare-metal Embedded C for ARM Cortex-M microcontrollers (STM32). The project bypasses high-level abstraction layers (like HAL or LL libraries) to handle memory-mapped registers directly, integrating the FreeRTOS kernel for deterministic multitasking.

## Execution Design
* **Hardware Register Layer:** Direct manipulation of `RCC_AHB1ENR`, `GPIOD_MODER`, and `GPIOD_ODR` registers for clock gating and ultra-low-latency peripheral configuration.
* **Kernel Task Multi-threading:** Execution of concurrent tasks managed by the FreeRTOS preemptive scheduler.
* **Resource Management:** Asynchronous non-blocking task blocking using `vTaskDelay` to optimize low-power CPU states.

## Production-Level Scalability Roadmap
This structural template provides the execution foundation for advanced hardware-software co-design. Upcoming modules include:
1. **DMA Implementation:** Offloading data transfer overhead from the CPU by implementing Direct Memory Access arrays directly for High-Speed SPI/UART sensor matrices.
2. **Interrupt-Driven Architecture:** Replacing periodic sampling loops with nested vector interrupt configurations (NVIC) to process real-world edge computations asynchronously.
3. **Power Profile Profiling:** Fine-tuning FreeRTOS low-power configurations to execute sleep/deep-sleep modes based on task priority hierarchies.
