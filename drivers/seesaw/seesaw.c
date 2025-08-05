#define DT_DRV_COMPAT adafruit_seesaw

#include <errno.h>
#include <zephyr/logging/log.h>

#include "seesaw.h"

LOG_MODULE_REGISTER(seesaw);


// int32_t (*getEncoderDelta)(const struct device *dev, uint8_t encoder);
static int32_t seesaw_get_encoder_delta(const struct device *dev, uint8_t encoder, int32_t *delta) {
    int32_t ret;

    const struct seesaw_config *cfg = (const struct seesaw_config *)dev->config;
    const struct i2c_dt_spec *i2c = &cfg->i2c;

    LOG_DBG("Hello from seesaw api!");

    return 0;
}

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

    return 0;
}


// --- DEVICETREE HANDLING ---
static const struct seesaw_api seesaw_api_funcs {
    .get_encoder_delta = seesaw_get_encoder_delta
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