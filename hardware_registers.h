// hardware_registers.h
// Custom register abstractions for bare-metal memory execution
#ifndef HARDWARE_REGISTERS_H
#define HARDWARE_REGISTERS_H

#define SENSOR_DATA_REG  (*(volatile uint32_t*)(0x40020C24))
#define SYSTEM_STATUS_REG (*(volatile uint32_t*)(0x40020C28))

#endif
