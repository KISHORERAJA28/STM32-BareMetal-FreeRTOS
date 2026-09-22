#ifndef SPI_DMA_DRIVER_H
#define SPI_DMA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

void spi1_dma_init(void);
bool spi1_dma_transmit(uint8_t *data, uint16_t length);

extern volatile bool spi1_dma_transfer_complete;

#endif
