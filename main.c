/**
 * ******************************************************************************
 * @file           : main.c
 * @brief          : Bare-Metal Architecture & Deterministic FreeRTOS Task Scheduling
 * @target         : STM32 (ARM Cortex-M4 Architecture)
 * ******************************************************************************
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Register Level Base Addresses for STM32F4 Discovery (example target board)
#define RCC_BASE       0x40023800
#define AHB1ENR_OFFSET 0x30
#define GPIOD_BASE     0x40020C00
#define MODER_OFFSET   0x00
#define ODR_OFFSET     0x14

// Precise Pointers to Direct Memory Access registers
volatile uint32_t* pRCC_AHB1ENR = (volatile uint32_t*)(RCC_BASE + AHB1ENR_OFFSET);
volatile uint32_t* pGPIOD_MODER = (volatile uint32_t*)(GPIOD_BASE + MODER_OFFSET);
volatile uint32_t* pGPIOD_ODR   = (volatile uint32_t*)(GPIOD_BASE + ODR_OFFSET);

// Function prototypes for FreeRTOS Tasks
void vSensorTelemetryTask(void *pvParameters);
void vActuatorControlTask(void *pvParameters);

int main(void)
{
    // Step 1: Bare-Metal Hardware Register Configuration
    *pRCC_AHB1ENR |= (1 << 3); // Enable Clock gating for GPIOD Peripherals
    *pGPIOD_MODER |= (1 << 24); // Set GPIOD Pin 12 as General Purpose Output Mode

    // Step 2: Initialize Deterministic RTOS Multi-threaded Tasks
    xTaskCreate(vSensorTelemetryTask, "Sensor_Read", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vActuatorControlTask, "Actuator_Ctrl", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Step 3: Core Kernel Boot Sequence Execution
    vTaskStartScheduler();

    while(1); // Dead loop context if scheduler initialization fails
}

void vSensorTelemetryTask(void *pvParameters)
{
    while(1)
    {
        // Custom bare-metal sensor read code execution via peripheral lines would go here
        vTaskDelay(pdMS_TO_TICKS(100)); // Relinquish CPU to lower priority tasks for 100ms
    }
}

void vActuatorControlTask(void *pvParameters)
{
    while(1)
    {
        *pGPIOD_ODR ^= (1 << 12); // Hardware Bit-toggling using memory registers
        vTaskDelay(pdMS_TO_TICKS(500)); // Fixed execution period
    }
}
