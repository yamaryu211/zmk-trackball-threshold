# ZMK Trackball Threshold Module

A ZMK input processor module that provides intelligent trackball movement filtering with AML (Auto Mouse Layer) state awareness, preventing accidental auto mouse layer activation from minor movements while maintaining responsive control when needed.

## Features

- **AML State-Aware Filtering**: Different behavior based on whether AML is active or inactive
- **Intelligent Threshold Control**: Prevents AML activation from small movements when AML is not active
- **Configurable Movement Threshold**: Customizable sensitivity settings
- **Transparent Pass-Through**: Normal operation when AML is already active
- **Compatible Integration**: Works seamlessly with existing ZMK input processor chains
- **Lightweight Implementation**: Minimal performance impact

## How It Works

### AML Inactive State

- **Small movements (≤ threshold)**: Discarded - prevents accidental AML activation
- **Large movements (> threshold)**: Passed through - allows intentional AML activation

### AML Active State

- **All movements**: Passed through normally - maintains responsive mouse control

This intelligent behavior ensures that:

1. Minor hand movements or vibrations don't accidentally trigger AML
2. Intentional trackball usage properly activates AML
3. Once AML is active, all movements work normally for precise control

## Installation

Add this module to your `west.yml`:

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    # Add your remote here
    - name: your-remote
      url-base: https://github.com/your-username
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    - name: zmk-trackball-threshold
      remote: your-remote
      revision: main
  self:
    path: config
```

## Usage

### Basic Configuration

```c
/ {
    zip_trackball_threshold: zip_trackball_threshold {
        compatible = "zmk,input-processor-trackball-threshold";
        #input-processor-cells = <0>;
        movement-threshold = <5>; // Optional, defaults to Kconfig value
    };
};

&trackball_listener {
    input-processors = <&zip_trackball_threshold>, <&zip_temp_layer AML 30000>;
};
```

### Configuration Options

- `movement-threshold`: Minimum movement threshold (default: 5)
  - Movement is calculated as `abs(x) + abs(y)`
  - When AML is inactive: values below this threshold are discarded
  - When AML is active: all values are passed through
- `#input-processor-cells`: Must be set to `<0>` (no parameters)

### Kconfig Options

```
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD=y
CONFIG_ZMK_INPUT_PROCESSOR_TRACKBALL_THRESHOLD_DEFAULT_THRESHOLD=5
```

## Important Notes

- This module assumes AML is defined as layer 8 in your keymap
- The module must be placed **before** `zip_temp_layer` in the input processor chain
- Requires `#include <zmk/keymap.h>` support in your ZMK build

## License

MIT License
