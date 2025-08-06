#define DT_DRV_COMPAT adafruit_seesaw

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "seesaw.h"

LOG_MODULE_REGISTER(seesaw);

// -- BEGIN PRIVATE FUNCTIONS ---
static int seesaw_write(const struct device *dev, uint8_t base, uint8_t function, uint8_t *buf, uint32_t num_bytes) {
    int ret;
    const struct seesaw_config *cfg = (const struct seesaw_config *)dev->config;

    uint8_t data[2 + num_bytes];
    data[0] = base;
    data[1] = function;

    uint32_t i;
    for (i = 0; i < num_bytes; i++) {
        data[i + 2] = buf[i]; 
    }


    ret = i2c_write_dt(&cfg->i2c, data, num_bytes + 2);
    if (ret != 0) {
        LOG_ERR("Error writing to seesaaw: %d", ret);
        return ret;
    }

    return 0;
}

static int seesaw_read(const struct device *dev, uint8_t base, uint8_t function, uint8_t *buf, uint32_t num_bytes, uint16_t delay_ms) {
    int ret;

    const struct seesaw_config *cfg = (const struct seesaw_config *)dev->config;
    // const struct i2c_dt_spec *i2c = &cfg->i2c;

    // write next read to seesaw
    uint8_t target[2] = {base, function};

    ret = i2c_write_dt(&cfg->i2c, target, 2);

    if (ret != 0) {
        LOG_ERR("Error occurred while writing to seesaw: %d", ret);
        return -EINVAL;
    } 

    k_sleep(K_MSEC(delay_ms));

    // read from seesaw
    ret = i2c_read_dt(&cfg->i2c, buf, num_bytes);

    k_sleep(K_MSEC(delay_ms));

    return 0;
}

static int seesaw_log_err_msg_quit(int err) {
    LOG_ERR("Error occurred while processing command: %d", err);
    return err;
}
// -- END PRIVATE FUNCTIONS ---

// --- BEGIN API FUNCTIONS ---
static int seesaw_set_pin_mode_bulk(const struct device *dev, uint32_t pins_port_a, uint32_t pins_port_b, uint8_t mode) {
    uint8_t cmd[] = {
        (uint8_t)(pins_port_a >> 24),
        (uint8_t)(pins_port_a >> 16),
        (uint8_t)(pins_port_a >> 8),
        (uint8_t)pins_port_a,
        (uint8_t)(pins_port_b >> 24),
        (uint8_t)(pins_port_b >> 16),
        (uint8_t)(pins_port_b >> 8),
        (uint8_t)pins_port_b
    };

    int ret;
    switch(mode) {
    case SEESAW_OUTPUT:
        return seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 8);
    case SEESAW_INPUT:
        return seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
    case SEESAW_INPUT_PULLUP:
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        return ret;
    case SEESAW_INPUT_PULLDOWN:
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        ret = seesaw_write(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 8);
        if (ret != 0) {
            return seesaw_log_err_msg_quit(ret);
        }
        return ret;
    }

    // Error if command not processed
    return -EINVAL;
}


static int seesaw_set_pin_mode(const struct device *dev, uint8_t pin, uint8_t mode) {
    if (pin >= 32) {
        return seesaw_set_pin_mode_bulk(dev, 0, 1ul << (pin - 32), mode);
    }
    return seesaw_set_pin_mode_bulk(dev, 1ul << pin, 0, mode);
}

static int seesaw_enable_encoder_switch(const struct device *dev) {
    return seesaw_set_pin_mode(dev, SEESAW_ENCODER_SWITCH_PIN, SEESAW_INPUT_PULLUP);
}

static int seesaw_read_gpio_port_a_bulk(const struct device *dev, uint32_t pins, uint32_t *pin_data) {
    uint8_t buf[4];
    int ret = seesaw_read(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4, 10);

    if (ret != 0) {
        LOG_ERR("Error reading from GPIO Port A Pins: %d", ret);
        return ret;
    }

    *pin_data = (
        (uint32_t)(buf[0] << 24) |
        (uint32_t)(buf[1] << 16) |
        (uint32_t)(buf[2] << 8) |
        (uint32_t)buf[3] 
    ) & pins;

    return 0;
}

static int seesaw_read_gpio_port_b_bulk(const struct device *dev, uint32_t pins, uint32_t *pin_data) {
    uint8_t buf[8];
    int ret = seesaw_read(dev, SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 8, 10);

    if (ret != 0) {
        LOG_ERR("Error reading from GPIO Port A Pins: %d", ret);
        return ret;
    }

    *pin_data = (
        (uint32_t)(buf[4] << 24) |
        (uint32_t)(buf[5] << 16) |
        (uint32_t)(buf[6] << 8) |
        (uint32_t)buf[7] 
    ) & pins;

    return 0;
}

static int seesaw_read_gpio_pin(const struct device *dev, uint8_t pin, uint8_t *status) {
    uint32_t pin_data;
    int ret;
    if (pin >= 32) {
        ret = seesaw_read_gpio_port_b_bulk(dev, (1ul << (pin - 32)), &pin_data);
    } else {
        ret = seesaw_read_gpio_port_a_bulk(dev, (1ul << pin), &pin_data);
    }

    if (ret != 0) {
        LOG_ERR("Error occurred while reading from pin %d: %d", pin, ret);
        return ret;
    }

    *status = (pin_data == 0) ? 0 : 1;

    return 0;
}

static int seesaw_get_encoder_button_status(const struct device *dev, uint8_t *status) {
    return seesaw_read_gpio_pin(dev, SEESAW_ENCODER_SWITCH_PIN, status);
}

static int seesaw_get_encoder_delta(const struct device *dev, uint8_t encoder, int32_t *delta) {
    int ret;

    uint32_t num_bytes = 4;
    int delay_ms = 10;
    uint8_t buf[num_bytes];

    ret = seesaw_read(dev, SEESAW_ENCODER_BASE, SEESAW_ENCODER_DELTA, buf, num_bytes, delay_ms);

    if (ret != 0) {
        LOG_ERR("Error occurred while retrieving encoder delta: %d", ret);
        return ret;
    }

    *delta = (int32_t)(
        ((uint32_t)buf[0] << 24) |
        ((uint32_t)buf[1] << 16) |
        ((uint32_t)buf[2] << 8) |
        (uint32_t)buf[3]
    );
    LOG_DBG("Delta: %d", *delta);

    return 0;
}
// --- END API FUNCTIONS ---

// Initialize the Seesaw
static int seesaw_init(const struct device *dev) {
    // int ret;

    // cast device config to seesaw config
    const struct seesaw_config *cfg = (const struct seesaw_config *)dev->config;

    // Get i2c struct from the config
    // const struct i2c_dt_spec *i2c = &cfg->i2c;

    // if (!i2c_is_ready_dt(i2c)) {
    //     LOG_ERR("I2C is not ready");
    //     return -ERNODEV;
    // }

    // Check if i2c bus is ready
    if (!device_is_ready(cfg->i2c.bus)) {
        LOG_ERR("I2C bus device not ready");
        return -EINVAL;
    }

    LOG_INF("Hello from seesaw api!");

    return 0;
}


// --- DEVICETREE HANDLING ---
static const struct seesaw_api seesaw_api_funcs = {
    .get_encoder_delta = seesaw_get_encoder_delta,
    .enable_encoder_switch = seesaw_enable_encoder_switch,
    .get_encoder_button_status = seesaw_get_encoder_button_status,
    .set_pin_mode_bulk = seesaw_set_pin_mode_bulk,
    .set_pin_mode = seesaw_set_pin_mode,
    .read_gpio_port_a_bulk = seesaw_read_gpio_port_a_bulk,
    .read_gpio_port_b_bulk = seesaw_read_gpio_port_b_bulk,
    .read_gpio_pin = seesaw_read_gpio_pin
};

#define SEESAW_DEFINE(inst) \
    /* Create an instance of the config struct, populate with DT values */ \
    static const struct seesaw_config seesaw_config_##inst = { \
        .i2c = I2C_DT_SPEC_INST_GET(inst), \
        .id = inst \
    }; \
    /* Create a "device" instance from a Devicetree node identifier */ \
    DEVICE_DT_INST_DEFINE(inst, \
                    &seesaw_init, \
                    NULL, \
                    NULL, \
                    &seesaw_config_##inst, \
                    POST_KERNEL, \
                    CONFIG_I2C_INIT_PRIORITY, \
                    &seesaw_api_funcs); \

DT_INST_FOREACH_STATUS_OKAY(SEESAW_DEFINE);