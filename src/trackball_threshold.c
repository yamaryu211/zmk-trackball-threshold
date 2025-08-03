/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_trackball_threshold

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/input_processors.h>
#include <zmk/trackball_threshold.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int trackball_threshold_process(const struct device *dev, struct input_event *event,
                                     uint32_t param, struct zmk_input_processor_state *state) {
    const struct trackball_threshold_config *config = dev->config;
    
    // Only process mouse movement events
    if (event->code != INPUT_REL_X && event->code != INPUT_REL_Y) {
        // Pass through non-movement events unchanged
        return zmk_input_processor_state_add_event(state, *event);
    }
    
    // Accumulate movement for threshold calculation
    static int16_t accumulated_x = 0;
    static int16_t accumulated_y = 0;
    
    if (event->code == INPUT_REL_X) {
        accumulated_x += event->value;
    } else if (event->code == INPUT_REL_Y) {
        accumulated_y += event->value;
    }
    
    // Calculate total movement magnitude
    uint16_t movement_magnitude = abs(accumulated_x) + abs(accumulated_y);
    
    // Check if movement exceeds threshold
    if (movement_magnitude > config->movement_threshold) {
        // Movement exceeds threshold, pass through accumulated movement
        struct input_event x_event = {
            .type = INPUT_EV_REL,
            .code = INPUT_REL_X,
            .value = accumulated_x
        };
        struct input_event y_event = {
            .type = INPUT_EV_REL,
            .code = INPUT_REL_Y,
            .value = accumulated_y
        };
        
        // Reset accumulation
        accumulated_x = 0;
        accumulated_y = 0;
        
        // Add both events to state
        int ret = zmk_input_processor_state_add_event(state, x_event);
        if (ret < 0) {
            return ret;
        }
        return zmk_input_processor_state_add_event(state, y_event);
    }
    
    // Movement below threshold, discard (don't add to state)
    return 0;
}

static int trackball_threshold_init(const struct device *dev) {
    LOG_DBG("Trackball threshold input processor initialized");
    return 0;
}

static const struct zmk_input_processor_driver_api trackball_threshold_driver_api = {
    .process = trackball_threshold_process,
};

#define TRACKBALL_THRESHOLD_INST(n)                                                               \
    static const struct trackball_threshold_config trackball_threshold_config_##n = {             \
        .movement_threshold = DT_INST_PROP_OR(n, movement_threshold,                              \
                                            CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD_DEFAULT_THRESHOLD), \
    };                                                                                            \
    static struct trackball_threshold_data trackball_threshold_data_##n = {};                     \
    DEVICE_DT_INST_DEFINE(n, trackball_threshold_init, NULL, &trackball_threshold_data_##n,       \
                         &trackball_threshold_config_##n, POST_KERNEL,                           \
                         CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &trackball_threshold_driver_api);

DT_INST_FOREACH_STATUS_OKAY(TRACKBALL_THRESHOLD_INST) 