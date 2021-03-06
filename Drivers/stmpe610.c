//
// Created by paul on 20.01.18.
//
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "stmpe610.h"
#include "../HAL/spi.h"

/*
 * CS: Port 8 -> PB0
 */
#define STMPE610_CS_PORT PORTB
#define STMPE610_CS_DDR DDRB
#define STMPE610_CS_BIT 0

#define STMPE_ADDR 0x41

#define STMPE_SYS_CTRL1 0x03
#define STMPE_SYS_CTRL1_RESET 0x02
#define STMPE_SYS_CTRL2 0x04

#define STMPE_TSC_CTRL 0x40
#define STMPE_TSC_CTRL_EN 0x01
#define STMPE_TSC_CTRL_XYZ 0x00
#define STMPE_TSC_CTRL_XY 0x02

#define STMPE_INT_CTRL 0x09
#define STMPE_INT_CTRL_POL_HIGH 0x04
#define STMPE_INT_CTRL_POL_LOW 0x00
#define STMPE_INT_CTRL_EDGE 0x02
#define STMPE_INT_CTRL_LEVEL 0x00
#define STMPE_INT_CTRL_ENABLE 0x01
#define STMPE_INT_CTRL_DISABLE 0x00



#define STMPE_INT_EN 0x0A
#define STMPE_INT_EN_TOUCHDET 0x01
#define STMPE_INT_EN_FIFOTH 0x02
#define STMPE_INT_EN_FIFOOF 0x04
#define STMPE_INT_EN_FIFOFULL 0x08
#define STMPE_INT_EN_FIFOEMPTY 0x10
#define STMPE_INT_EN_ADC 0x40
#define STMPE_INT_EN_GPIO 0x80

#define STMPE_INT_STA 0x0B
#define STMPE_INT_STA_TOUCHDET 0x01

#define STMPE_ADC_CTRL1 0x20
#define STMPE_ADC_CTRL1_12BIT 0x08
#define STMPE_ADC_CTRL1_10BIT 0x00

#define STMPE_ADC_CTRL2 0x21
#define STMPE_ADC_CTRL2_1_625MHZ 0x00
#define STMPE_ADC_CTRL2_3_25MHZ 0x01
#define STMPE_ADC_CTRL2_6_5MHZ 0x02

#define STMPE_TSC_CFG 0x41
#define STMPE_TSC_CFG_1SAMPLE 0x00
#define STMPE_TSC_CFG_2SAMPLE 0x40
#define STMPE_TSC_CFG_4SAMPLE 0x80
#define STMPE_TSC_CFG_8SAMPLE 0xC0
#define STMPE_TSC_CFG_DELAY_10US 0x00
#define STMPE_TSC_CFG_DELAY_50US 0x08
#define STMPE_TSC_CFG_DELAY_100US 0x10
#define STMPE_TSC_CFG_DELAY_500US 0x18
#define STMPE_TSC_CFG_DELAY_1MS 0x20
#define STMPE_TSC_CFG_DELAY_5MS 0x28
#define STMPE_TSC_CFG_DELAY_10MS 0x30
#define STMPE_TSC_CFG_DELAY_50MS 0x38
#define STMPE_TSC_CFG_SETTLE_10US 0x00
#define STMPE_TSC_CFG_SETTLE_100US 0x01
#define STMPE_TSC_CFG_SETTLE_500US 0x02
#define STMPE_TSC_CFG_SETTLE_1MS 0x03
#define STMPE_TSC_CFG_SETTLE_5MS 0x04
#define STMPE_TSC_CFG_SETTLE_10MS 0x05
#define STMPE_TSC_CFG_SETTLE_50MS 0x06
#define STMPE_TSC_CFG_SETTLE_100MS 0x07

#define STMPE_FIFO_TH 0x4A

#define STMPE_FIFO_SIZE 0x4C

#define STMPE_FIFO_STA 0x4B
#define STMPE_FIFO_STA_RESET 0x01
#define STMPE_FIFO_STA_OFLOW 0x80
#define STMPE_FIFO_STA_FULL 0x40
#define STMPE_FIFO_STA_EMPTY 0x20
#define STMPE_FIFO_STA_THTRIG 0x10

#define STMPE_TSC_I_DRIVE 0x58
#define STMPE_TSC_I_DRIVE_20MA 0x00
#define STMPE_TSC_I_DRIVE_50MA 0x01

#define STMPE_TSC_DATA_X 0x4D
#define STMPE_TSC_DATA_Y 0x4F
#define STMPE_TSC_FRACTION_Z 0x56

#define STMPE_GPIO_SET_PIN 0x10
#define STMPE_GPIO_CLR_PIN 0x11
#define STMPE_GPIO_DIR 0x13
#define STMPE_GPIO_ALT_FUNCT 0x17

static volatile bool ready = false;

void spi_callback(void) {
    ready = true;
}

void stmpe610_pre_spi_init(void) {
    STMPE610_CS_DDR |= (1 << STMPE610_CS_BIT);
    STMPE610_CS_PORT |= (1 << STMPE610_CS_BIT);
}

bool stmpe610_post_spi_init(void) {
    uint16_t version = stmpe610_get_version();
    if (version != 0x811) {
        return false;
    }
    stmpe610_write_register8(STMPE_SYS_CTRL1, STMPE_SYS_CTRL1_RESET);
    _delay_ms(10);

    for (uint8_t i=0; i<65; i++) {
        stmpe610_read_register8(i);
    }

    stmpe610_write_register8(STMPE_SYS_CTRL2, 0x0); // turn on clocks!
    stmpe610_write_register8(STMPE_TSC_CTRL, STMPE_TSC_CTRL_XYZ | STMPE_TSC_CTRL_EN); // XYZ and enable!
    stmpe610_write_register8(STMPE_INT_EN, STMPE_INT_EN_TOUCHDET);
    stmpe610_write_register8(STMPE_ADC_CTRL1, STMPE_ADC_CTRL1_10BIT | (0x6 << 4)); // 96 clocks per conversion
    stmpe610_write_register8(STMPE_ADC_CTRL2, STMPE_ADC_CTRL2_6_5MHZ);
    stmpe610_write_register8(STMPE_TSC_CFG, STMPE_TSC_CFG_4SAMPLE | STMPE_TSC_CFG_DELAY_1MS | STMPE_TSC_CFG_SETTLE_5MS);
    stmpe610_write_register8(STMPE_TSC_FRACTION_Z, 0x6);
    stmpe610_write_register8(STMPE_FIFO_TH, 1);
    stmpe610_write_register8(STMPE_FIFO_STA, STMPE_FIFO_STA_RESET);
    stmpe610_write_register8(STMPE_FIFO_STA, 0);    // unreset
    stmpe610_write_register8(STMPE_TSC_I_DRIVE, STMPE_TSC_I_DRIVE_50MA);
    stmpe610_write_register8(STMPE_INT_STA, 0xFF); // reset all ints
    stmpe610_write_register8(STMPE_INT_CTRL, STMPE_INT_CTRL_POL_HIGH | STMPE_INT_CTRL_ENABLE);
    return true;
}

uint8_t stmpe610_touched(void) {
    return (stmpe610_read_register8(STMPE_TSC_CTRL) & 0x80);

}

uint8_t stmpe610_buffer_empty(void) {
    return (stmpe610_read_register8(STMPE_FIFO_STA) & STMPE_FIFO_STA_EMPTY);
}

uint8_t stmpe610_buffer_size(void) {
    return stmpe610_read_register8(STMPE_FIFO_SIZE);
}

tsPoint_t stmpe610_get_point(void) {
    uint16_t x, y;
    uint8_t z;
    stmpe610_read_data(&x, &y, &z);
    return (tsPoint_t){x, y, z};
}

void stmpe610_write_register8(uint8_t reg, uint8_t val) {
    STMPE610_CS_PORT &= ~(1 << STMPE610_CS_BIT);
    _delay_us(5);//little delay
    uint8_t buf[2] = {reg, val};
    ready = false;
    spi_tx_rx(buf, 2, &spi_callback);
    while (!ready);
    STMPE610_CS_PORT |= (1<<STMPE610_CS_BIT);
}

uint16_t stmpe610_read_register16(uint8_t reg) {
    STMPE610_CS_PORT &= ~(1 << STMPE610_CS_BIT);
    _delay_us(5);//little delay
    uint8_t buf[3] = {0x80 | reg, 0x00, 0x00};
    ready = false;
    spi_tx_rx(buf, 3, &spi_callback);
    while (!ready);
    STMPE610_CS_PORT |= (1<<STMPE610_CS_BIT);
    return buf[1] << 8u | buf[2];
}

uint8_t stmpe610_read_register8(uint8_t reg) {
    STMPE610_CS_PORT &= ~(1 << STMPE610_CS_BIT);
    _delay_us(5);//little delay
    uint8_t buf[2] = {0x80 | reg, 0x00};
    ready = false;
    spi_tx_rx(buf, 2, &spi_callback);
    while (!ready);
    STMPE610_CS_PORT |= (1<<STMPE610_CS_BIT);
    return buf[1];
}

void stmpe610_read_data(uint16_t *x, uint16_t *y, uint8_t *z) {
    uint8_t data[4];

    for (uint8_t i=0; i<4; i++) {
        data[i] = stmpe610_read_register8(0xD7);
    }
    *x = data[0];
    *x <<= 4;
    *x |= (data[1] >> 4);
    *y = data[1] & 0x0F;
    *y <<= 8;
    *y |= data[2];
    *z = data[3];

    if (stmpe610_buffer_empty()) {
        stmpe610_write_register8(STMPE_INT_STA, 0xFF); // reset all ints
    }
}

uint16_t stmpe610_get_version(void) {
    uint16_t v;
    v = stmpe610_read_register8(0);
    v <<= 8;
    v |= stmpe610_read_register8(1);
    return v;
}

