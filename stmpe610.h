//
// Created by paul on 20.01.18.
//

#ifndef RADIOCONTROLSOFTWARE_STMPE610_H
#define RADIOCONTROLSOFTWARE_STMPE610_H

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

typedef struct TsPoint {
    int16_t x,y,z;
} tsPoint_t;

void stmpe610_init();
uint8_t stmpe610_touched();
uint8_t stmpe610_buffer_empty();
uint8_t stmpe610_buffer_Size();
tsPoint_t stmpe610_get_point();

void stmpe610_writeRegister8(uint8_t reg, uint8_t val);
uint16_t stmpe610_readRegister16(uint8_t reg);
uint8_t stmpe610_readRegister8(uint8_t reg);
void stmpe610_readData(uint16_t *x, uint16_t *y, uint8_t *z);
uint16_t stmpe610_getVersion();



#endif //RADIOCONTROLSOFTWARE_STMPE610_H
