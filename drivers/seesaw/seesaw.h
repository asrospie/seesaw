#ifndef ZEPHYR_DRIVERS_SEESAW_H
#define ZEPHYR_DRIVERS_SEESAW_H

#include <zephyr/drivers/i2c.h>

// Module Base Addresses
#define SEESAW_STATUS_BASE 0x00
#define SEESAW_GPIO_BASE 0x01
#define SEESAW_SERCOM0_BASE 0x02
#define SEESAW_TIMER_BASE 0x08
#define SEESAW_ADC_BASE 0x09
#define SEESAW_DAC_BASE 0x0A
#define SEESAW_INTERRUPT_BASE 0x0B
#define SEESAW_DAP_BASE 0x0C
#define SEESAW_EEPROM_BASE 0x0D
#define SEESAW_NEOPIXEL_BASE 0x0E
#define SEESAW_TOUCH_BASE 0x0F
#define SEESAW_KEYPAD_BASE 0x10
#define SEESAW_ENCODER_BASE 0x11
#define SEESAW_SPECTRUM_BASE 0x012

// --- BEGIN GPIO MODULE FUNCTION ADDRESS REGISTERS ---
#define SEESAW_GPIO_DIRSET_BULK 0x02
#define SEESAW_GPIO_DIRCLR_BULK 0x03
#define SEESAW_GPIO_BULK 0x04
#define SEESAW_GPIO_BULK_SET 0x05
#define SEESAW_GPIO_BULK_CLR 0x06
#define SEESAW_GPIO_BULK_TOGGLE 0x07
#define SEESAW_GPIO_INTENSET 0x08
#define SEESAW_GPIO_INTENCLR 0x09
#define SEESAW_GPIO_INTFLAG 0x0A
#define SEESAW_GPIO_PULLENSET 0x0B
#define SEESAW_GPIO_PULLENCLR 0x0C
// --- END GPIO MODULE FUNCTION ADDRESS REGISTERS ---

// --- BEGIN ENCODER MODULE FUNCTION ADDRESS REGISTERS ---
#define SEESAW_ENCODER_STATUS 0x00
#define SEESAW_ENCODER_INTENSET 0x10
#define SEESAW_ENCODER_INTENCLR 0x20
#define SEESAW_ENCODER_POSITION 0x30
#define SEESAW_ENCODER_DELTA 0x40
// --- END ENCODER MODULE FUNCTION ADDRESS REGISTERS ---

// Defining custom Seesaw API
struct seesaw_api {
    // GPIO API
    // void (*pinMode)(const struct device *dev, uint8_t pin, uint8_t mode);
    // void (*pinModeBulk)(const struct device *dev, uint32_t pins, uint8_t mode);
    // void (*pinWrite)(const struct device *dev, uint8_t pin, uint8_t value);
    // int32_t (*pinRead)(const struct device *dev, uint8_t pin);

    // Encoder API
    // int32_t (*getEncoderPostion)(const struct device *dev, uint8_t encoder);
    int32_t (*get_encoder_delta)(const struct device *dev, uint8_t encoder, int32_t *delta);
    // bool (*enableEncoderInterrupt)(const struct device *dev, uint8_t encoder);
    // bool (*disableEncoderInterrupt)(const struct device *dev, uint8_t encoder);
};


// Configuration
struct seesaw_config {
    struct i2c_dt_spec i2c;
    uint32_t id;
};


#endif