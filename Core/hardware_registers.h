// ====================================================================
// hardware_registers.h
// Custom register-level base addresses and bit definitions.
// Target: STM32F4 (Cortex-M4) family — verify exact offsets against
// your specific part's reference manual (RM0090 for F405/F407/F429).
// ====================================================================
#ifndef HARDWARE_REGISTERS_H
#define HARDWARE_REGISTERS_H

#include <stdint.h>

// ---------------- RCC ----------------
#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x44))

#define RCC_AHB1ENR_DMA2EN   (1 << 22)
#define RCC_APB2ENR_SPI1EN   (1 << 12)

// ---------------- GPIOD (existing) ----------------
#define GPIOD_BASE      0x40020C00UL
#define GPIOD_MODER     (*(volatile uint32_t*)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t*)(GPIOD_BASE + 0x14))

// ---------------- SPI1 ----------------
#define SPI1_BASE       0x40013000UL
#define SPI1_CR1        (*(volatile uint32_t*)(SPI1_BASE + 0x00))
#define SPI1_CR2        (*(volatile uint32_t*)(SPI1_BASE + 0x04))
#define SPI1_SR         (*(volatile uint32_t*)(SPI1_BASE + 0x08))
#define SPI1_DR         (*(volatile uint32_t*)(SPI1_BASE + 0x0C))

#define SPI_CR1_SPE     (1 << 6)   // SPI Enable
#define SPI_CR1_MSTR    (1 << 2)   // Master mode
#define SPI_CR2_TXDMAEN (1 << 1)   // Tx buffer DMA enable
#define SPI_SR_TXE      (1 << 1)

// ---------------- DMA2 Stream3 (SPI1_TX on most STM32F4 parts) ----------------
// NOTE: verify the DMA request mapping table in your part's reference
// manual — SPI1_TX may map to a different stream/channel depending on
// the exact STM32F4 variant.
#define DMA2_BASE           0x40026400UL
#define DMA2_S3_BASE        (DMA2_BASE + 0x10 + (0x18 * 3))

#define DMA2_S3CR       (*(volatile uint32_t*)(DMA2_S3_BASE + 0x00))
#define DMA2_S3NDTR     (*(volatile uint32_t*)(DMA2_S3_BASE + 0x04))
#define DMA2_S3PAR      (*(volatile uint32_t*)(DMA2_S3_BASE + 0x08))
#define DMA2_S3M0AR     (*(volatile uint32_t*)(DMA2_S3_BASE + 0x0C))

#define DMA_SxCR_CHSEL3 (3 << 25)  // Channel 3
#define DMA_SxCR_MINC   (1 << 10)  // Memory increment mode
#define DMA_SxCR_DIR_M2P (1 << 6)  // Memory-to-peripheral
#define DMA_SxCR_TCIE   (1 << 4)   // Transfer complete interrupt enable
#define DMA_SxCR_EN     (1 << 0)   // Stream enable

// ---------------- NVIC ----------------
#define NVIC_ISER1      (*(volatile uint32_t*)0xE000E104UL) // IRQs 32-63

// DMA2_Stream3_IRQn = 58 on STM32F4 -> bit (58-32)=26 in ISER1.
// CONFIRM this against your device's startup_stm32f4xx.s / vector table
// before relying on it — IRQ numbering can differ by part variant.
#define DMA2_STREAM3_IRQ_BIT   (1 << 26)

#endif
