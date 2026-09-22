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
    //Bare-Metal Hardware Configuration.
    RCC_AHB1ENR |= (1 << 3);   
    GPIOD_MODER |= (1 << 24); 

    //Interrupt-driven,DMA-backedSPI1 initialization.
    spi1_dma_init();

    //RTOS synchronization primitive for ISR -> task handoff.
    xSpiDmaCompleteSemaphore = xSemaphoreCreateBinary();

    //Initialize deterministic RTOS multi-threaded tasks.
    xTaskCreate(vSensorTelemetryTask, "Sensor_Read", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vActuatorControlTask, "Actuator_Ctrl", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vSpiDmaConsumerTask, "SPI_DMA_Consumer", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    //Core kernel boot sequence.
    vTaskStartScheduler();
    while (1); //if init fails..
}

void vSensorTelemetryTask(void *pvParameters)
{
    while (1) {
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

//demonstrates interrupt -> RTOS task synchronization, not polling.
void vSpiDmaConsumerTask(void *pvParameters)
{
    while (1) {
        if (xSemaphoreTake(xSpiDmaCompleteSemaphore, portMAX_DELAY) == pdTRUE) {
            spi1_dma_transfer_complete = false;
        }
    }
}
