#include "spi_dma_driver.h"
#include "hardware_registers.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t xSpiDmaCompleteSemaphore;
volatile bool spi1_dma_transfer_complete = false;

void spi1_dma_init(void)
{
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    SPI1_CR1 |= SPI_CR1_MSTR;
    SPI1_CR2 |= SPI_CR2_TXDMAEN;
    SPI1_CR1 |= SPI_CR1_SPE;
    NVIC_ISER1 = DMA2_STREAM3_IRQ_BIT;
}

bool spi1_dma_transmit(uint8_t *data, uint16_t length)
{
    if (DMA2_S3CR & DMA_SxCR_EN) {
        return false; 
    }

    DMA2_S3PAR  = (uint32_t)&SPI1_DR;
    DMA2_S3M0AR = (uint32_t)data;
    DMA2_S3NDTR = length;

    DMA2_S3CR = DMA_SxCR_CHSEL3 | DMA_SxCR_MINC | DMA_SxCR_DIR_M2P
              | DMA_SxCR_TCIE   | DMA_SxCR_EN;

    return true;
}
void DMA2_Stream3_IRQHandler(void)
{
    *(volatile uint32_t*)(DMA2_BASE + 0x0C) = (1 << 27); /

    spi1_dma_transfer_complete = true;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSpiDmaCompleteSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
