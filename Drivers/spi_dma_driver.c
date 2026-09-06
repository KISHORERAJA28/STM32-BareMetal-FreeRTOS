// ====================================================================
// spi_dma_driver.c
// Description: Register-level SPI1 + DMA2 Stream3 driver for
// interrupt-driven, CPU-offloaded transmit transfers.
//
// Design:
//   - SPI1 configured as master, DMA-driven TX (no CPU byte-by-byte
//     writes to SPI1_DR).
//   - DMA2 Stream3 configured memory -> peripheral, with transfer-
//     complete interrupt enabled.
//   - ISR hands off to a FreeRTOS task via a binary semaphore
//     (see main.c), keeping the ISR itself minimal.
// ====================================================================
#include "spi_dma_driver.h"
#include "hardware_registers.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t xSpiDmaCompleteSemaphore;
volatile bool spi1_dma_transfer_complete = false;

void spi1_dma_init(void)
{
    // 1. Enable peripheral clocks
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // 2. Configure SPI1: master mode, DMA TX enable
    SPI1_CR1 |= SPI_CR1_MSTR;
    SPI1_CR2 |= SPI_CR2_TXDMAEN;
    SPI1_CR1 |= SPI_CR1_SPE;

    // 3. Enable the DMA stream's transfer-complete interrupt at NVIC
    NVIC_ISER1 = DMA2_STREAM3_IRQ_BIT;
}

bool spi1_dma_transmit(uint8_t *data, uint16_t length)
{
    if (DMA2_S3CR & DMA_SxCR_EN) {
        return false; // previous transfer still in flight
    }

    // Peripheral address = SPI1 data register
    DMA2_S3PAR  = (uint32_t)&SPI1_DR;
    // Memory address = source buffer
    DMA2_S3M0AR = (uint32_t)data;
    // Number of bytes to transfer
    DMA2_S3NDTR = length;

    // Channel 3, memory-increment, memory->peripheral, TC interrupt, enable
    DMA2_S3CR = DMA_SxCR_CHSEL3 | DMA_SxCR_MINC | DMA_SxCR_DIR_M2P
              | DMA_SxCR_TCIE   | DMA_SxCR_EN;

    return true;
}

// ---------------- Interrupt Service Routine ----------------
// Name must match the entry in your startup file's vector table
// (commonly DMA2_Stream3_IRQHandler for STM32F4 CMSIS startup files).
void DMA2_Stream3_IRQHandler(void)
{
    // Clear the transfer-complete flag in DMA2's high-interrupt-status
    // register (LIFCR/HIFCR — offset and bit depend on stream number;
    // verify against RM0090 Table for DMA2 stream 3's flag-clear bit).
    // Placeholder clear-flag write:
    *(volatile uint32_t*)(DMA2_BASE + 0x0C) = (1 << 27); // CTCIF3 (verify bit position)

    spi1_dma_transfer_complete = true;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSpiDmaCompleteSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
