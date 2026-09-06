// ====================================================================
// spi_dma_driver.h
// ====================================================================
#ifndef SPI_DMA_DRIVER_H
#define SPI_DMA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void spi1_dma_init(void);
bool spi1_dma_transmit(uint8_t *data, uint16_t length);

// Set by the DMA ISR, cleared by the task that consumes the transfer.
extern volatile bool spi1_dma_transfer_complete;

#endif
