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

// -- BEGIN ENCODER SPECIFIC DEFS
#define SEESAW_ENCODER_DEFAULT_ADDR 0x36
#define SEESAW_ENCODER_SWITCH_PIN 24
#define SEESAW_ENCODER_NEOPIX_PIN 6
// -- END ENCODER SPECIFIC DEFS

// --- BEGIN MISC. DEFINITIONS ---
#define SEESAW_INPUT 0x00
#define SEESAW_OUTPUT 0x01
#define SEESAW_INPUT_PULLUP 0x02
#define SEESAW_INPUT_PULLDOWN 0x03
// --- END MISC. DEFINITIONS ---

// Defining custom Seesaw API
struct seesaw_api {
    // Encoder API
    // int32_t (*get_encoder_position)(const struct device *dev, uint8_t encoder);
    int (*get_encoder_delta)(const struct device *dev, uint8_t encoder, int32_t *delta);
    int (*enable_encoder_switch)(const struct device *dev);
    int (*get_encoder_button_status)(const struct device *dev, uint8_t *status);

    // GPIO API
    int (*set_pin_mode_bulk)(const struct device *dev, uint32_t pins_port_a, uint32_t pins_port_b, uint8_t mode);
    int (*set_pin_mode)(const struct device *dev, uint8_t pin, uint8_t mode);
    int (*read_gpio_port_a_bulk)(const struct device *dev, uint32_t pins, uint32_t *pin_data);
    int (*read_gpio_port_b_bulk)(const struct device *dev, uint32_t pins, uint32_t *pin_data);
    int (*read_gpio_pin)(const struct device *dev, uint8_t pin, uint8_t *status);
};


// Configuration
struct seesaw_config {
    struct i2c_dt_spec i2c;
    uint32_t id;
};


#endif