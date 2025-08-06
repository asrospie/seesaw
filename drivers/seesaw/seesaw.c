#define DT_DRV_COMPAT adafruit_seesaw

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include "seesaw.h"

LOG_MODULE_REGISTER(seesaw);

// -- BEGIN PRIVATE FUNCTIONS ---
static int seesaw_read(const struct device *dev, uint8_t base, uint8_t function, uint8_t *buf, uint8_t num_bytes, uint16_t delay_ms) {
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
// -- END PRIVATE FUNCTIONS ---

// --- BEGIN API FUNCTIONS ---
static int seesaw_get_encoder_delta(const struct device *dev, uint8_t encoder, int32_t *delta) {
    int ret;

    int num_bytes = 4;
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
    LOG_INF("Delta: %d", *delta);

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