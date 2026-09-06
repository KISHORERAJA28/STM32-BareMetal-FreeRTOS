/**
 * ******************************************************************************
 * @file           : main.c
 * @brief          : Bare-Metal Architecture, Interrupt-Driven SPI/DMA, and
 *                   Deterministic FreeRTOS Task Scheduling
 * @target         : STM32 (ARM Cortex-M4 Architecture)
 * ******************************************************************************
 */
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware_registers.h"
#include "spi_dma_driver.h"

SemaphoreHandle_t xSpiDmaCompleteSemaphore;

void vSensorTelemetryTask(void *pvParameters);
void vActuatorControlTask(void *pvParameters);
void vSpiDmaConsumerTask(void *pvParameters);

static uint8_t telemetry_buffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int main(void)
{
    // Step 1: Bare-Metal Hardware Register Configuration
    RCC_AHB1ENR |= (1 << 3);   // Enable clock gating for GPIOD
    GPIOD_MODER |= (1 << 24); // GPIOD Pin 12 as general-purpose output

    // Step 2: Interrupt-driven, DMA-backed SPI1 initialization
    spi1_dma_init();

    // Step 3: RTOS synchronization primitive for ISR -> task handoff
    xSpiDmaCompleteSemaphore = xSemaphoreCreateBinary();

    // Step 4: Initialize deterministic RTOS multi-threaded tasks
    xTaskCreate(vSensorTelemetryTask, "Sensor_Read", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vActuatorControlTask, "Actuator_Ctrl", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vSpiDmaConsumerTask, "SPI_DMA_Consumer", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    // Step 5: Core kernel boot sequence
    vTaskStartScheduler();
    while (1); // Only reached if scheduler init fails
}

void vSensorTelemetryTask(void *pvParameters)
{
    while (1) {
        // Kick off a non-blocking, CPU-offloaded DMA transfer.
        spi1_dma_transmit(telemetry_buffer, sizeof(telemetry_buffer));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vActuatorControlTask(void *pvParameters)
{
    while (1) {
        GPIOD_ODR ^= (1 << 12);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Blocks on the semaphore given by the DMA ISR on transfer-complete —
// demonstrates interrupt -> RTOS task synchronization, not polling.
void vSpiDmaConsumerTask(void *pvParameters)
{
    while (1) {
        if (xSemaphoreTake(xSpiDmaCompleteSemaphore, portMAX_DELAY) == pdTRUE) {
            spi1_dma_transfer_complete = false;
            // Handle post-transfer logic here (e.g., mark buffer free).
        }
    }
}
