/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_trackball_threshold

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/input/input.h>

#include <zmk/trackball_threshold.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Helper function to add event to state
static int zmk_input_processor_state_add_event(struct zmk_input_processor_state *state, struct input_event event) {
    if (state->count >= state->capacity) {
        return -ENOMEM;
    }
    state->events[state->count++] = event;
    return 0;
}

// Helper function for absolute value
static inline uint16_t abs_value(int16_t val) {
    return val < 0 ? -val : val;
}

// Helper function to check if AML layer is active
static bool is_aml_active(void) {
    // AMLレイヤーは8番として定義されている
    return zmk_keymap_layer_active(8);
}

static int trackball_threshold_process(const struct device *dev, struct input_event *event,
                                     uint32_t param, struct zmk_input_processor_state *state) {
    const struct trackball_threshold_config *config = dev->config;
    struct trackball_threshold_data *data = dev->data;

    // Only process mouse movement events
    if (event->code != INPUT_REL_X && event->code != INPUT_REL_Y) {
        // Pass through non-movement events unchanged
        return zmk_input_processor_state_add_event(state, *event);
    }

    // Accumulate movement for threshold calculation
    if (event->code == INPUT_REL_X) {
        data->accumulated_x += event->value;
    } else if (event->code == INPUT_REL_Y) {
        data->accumulated_y += event->value;
    }

    // Calculate total movement magnitude
    uint16_t movement_magnitude = abs_value(data->accumulated_x) + abs_value(data->accumulated_y);

    // Check if AML is currently active
    bool aml_active = is_aml_active();

    // If AML is not active and movement is below threshold, discard the event
    // This prevents AML from being triggered by small movements
    if (!aml_active && movement_magnitude <= config->movement_threshold) {
        // Movement below threshold and AML not active, discard
        return 0;
    }

    // If we reach here, either:
    // 1. AML is already active (normal operation)
    // 2. Movement exceeds threshold (should trigger AML via zip_temp_layer)
    
    // Replace current event with accumulated value and reset accumulation
    if (event->code == INPUT_REL_X && data->accumulated_x != 0) {
        event->value = data->accumulated_x;
        data->accumulated_x = 0;
    } else if (event->code == INPUT_REL_Y && data->accumulated_y != 0) {
        event->value = data->accumulated_y;
        data->accumulated_y = 0;
    }

    // Reset both accumulations when processing movement
    data->accumulated_x = 0;
    data->accumulated_y = 0;

    // Forward the modified event
    return zmk_input_processor_state_add_event(state, *event);
}

static int trackball_threshold_init(const struct device *dev) {
    struct trackball_threshold_data *data = dev->data;
    
    // Initialize accumulated values
    data->accumulated_x = 0;
    data->accumulated_y = 0;
    
    LOG_DBG("Trackball threshold input processor initialized with threshold %d", 
            ((const struct trackball_threshold_config *)dev->config)->movement_threshold);
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