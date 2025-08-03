/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/device.h>
#include <zmk/input_processors.h>

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
    /* Currently no runtime data needed */
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