/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/device.h>

/**
 * @brief ZMK input processor state structure
 */
struct zmk_input_processor_state {
    struct input_event *events;
    size_t count;
    size_t capacity;
};

/**
 * @brief Trackball threshold input processor configuration
 */
struct trackball_threshold_config {
    uint16_t movement_threshold;
};

/**
 * @brief Trackball threshold input processor data
 */
struct trackball_threshold_data {
    int16_t accumulated_x;
    int16_t accumulated_y;
};

/**
 * @brief Process input event through trackball threshold filter
 *
 * @param dev Input processor device
 * @param event Input event to process
 * @param param Processing parameters
 * @param state Input processor state
 * @return 0 on success, negative errno on failure
 */
int trackball_threshold_process(const struct device *dev, struct input_event *event,
                               uint32_t param, struct zmk_input_processor_state *state); 